// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EquipmentInstance.h"

#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Equipment/EquipmentActor.h"
#include "Equipment/EquipmentDefinition.h"
#include "GameFramework/Character.h"

void UEquipmentInstance::OnEquipped()
{
}

void UEquipmentInstance::OnUnEquipped()
{
}

void UEquipmentInstance::SpawnEquipmentActors(const TArray<FEquipmentActorsToSpawn>& ActorsToSpawn)
{
	if (ACharacter* OwningCharacter = GetCharacter())
	{
		FStreamableManager& Manager = UAssetManager::GetStreamableManager();
		TWeakObjectPtr<UEquipmentInstance> WeakThis(this);
		
		for (const FEquipmentActorsToSpawn& ActorToSpawn : ActorsToSpawn)
		{
			if (IsValid(ActorToSpawn.EquipmentClass.Get()))
			{
				AEquipmentActor* NewActor = GetWorld()->SpawnActorDeferred<AEquipmentActor>(ActorToSpawn.EquipmentClass.Get(), FTransform::Identity,
						OwningCharacter);

				NewActor->FinishSpawning(FTransform::Identity);
				NewActor->AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, ActorToSpawn.AttachName);
				SpawnedActors.Emplace(NewActor);
			}
			else
			{
				Manager.RequestAsyncLoad(ActorToSpawn.EquipmentClass.ToSoftObjectPath(),
					[WeakThis, ActorToSpawn, OwningCharacter]
					{
						AEquipmentActor* NewActor = WeakThis->GetWorld()->SpawnActorDeferred<AEquipmentActor>(ActorToSpawn.EquipmentClass.Get(), FTransform::Identity,
						OwningCharacter);

						NewActor->FinishSpawning(FTransform::Identity);
						NewActor->AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, ActorToSpawn.AttachName);
						WeakThis->SpawnedActors.Emplace(NewActor);
					});
			}
		}
	}
}

void UEquipmentInstance::DestroySpawnedActors()
{
	for (AActor* Actor : SpawnedActors)
	{
		Actor->Destroy();
	}
}

ACharacter* UEquipmentInstance::GetCharacter()
{
	if (const APlayerController* PlayerController = Cast<APlayerController>(GetOuter()))
	{
		return Cast<ACharacter>(PlayerController->GetPawn());
	}

	return nullptr;
}