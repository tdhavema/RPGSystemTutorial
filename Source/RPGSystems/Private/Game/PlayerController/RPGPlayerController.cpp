// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/PlayerController/RPGPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Game/PlayerState/RPGPlayerState.h"
#include "Input/RPGSystemsInputComponent.h"
#include "Inventory/InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "UI/WidgetControllers/InventoryWidgetController.h"
#include "UI/RPGSystemsWidget.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Equipment/EquipmentManagerComponent.h"
#include "Inventory/ItemActor.h"

ARPGPlayerController::ARPGPlayerController()
{
	bReplicates = true;

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");
	InventoryComponent->SetIsReplicated(true);

	EquipmentComponent = CreateDefaultSubobject<UEquipmentManagerComponent>("EquipmentComponent");
}

void ARPGPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (URPGSystemsInputComponent* RPGInputComp = Cast<URPGSystemsInputComponent>(InputComponent))
	{
		RPGInputComp->BindAbilityActions(RPGInputConfig, this, &ThisClass::AbilityInputPressed, &ThisClass::AbilityInputReleased);
	}
}

void ARPGPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGPlayerController, InventoryComponent);
}

void ARPGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BindCallbacksToDependencies();
}

void ARPGPlayerController::AbilityInputPressed(FGameplayTag InputTag)
{
	if (IsValid(GetRPGAbilitySystemComponent()))
	{
		RPGAbilitySystemComp->AbilityInputPressed(InputTag);
	}
}

void ARPGPlayerController::AbilityInputReleased(FGameplayTag InputTag)
{
	if (IsValid(GetRPGAbilitySystemComponent()))
	{
		RPGAbilitySystemComp->AbilityInputReleased(InputTag);
	}
}

URPGAbilitySystemComponent* ARPGPlayerController::GetRPGAbilitySystemComponent()
{
	if (!IsValid(RPGAbilitySystemComp))
	{
		if (const ARPGPlayerState* RPGPlayerState = GetPlayerState<ARPGPlayerState>())
		{
			RPGAbilitySystemComp = RPGPlayerState->GetRPGAbilitySystemComponent();
		}
	}

	return RPGAbilitySystemComp;
}

void ARPGPlayerController::BindCallbacksToDependencies()
{
	if (IsValid(InventoryComponent) && IsValid(EquipmentComponent))
	{
		InventoryComponent->EquipmentItemDelegate.AddLambda(
			[this] (const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition, const FEquipmentEffectPackage& EffectPackage)
			{
				if (IsValid(EquipmentComponent))
				{
					EquipmentComponent->EquipItem(EquipmentDefinition, EffectPackage);
				}
			});

		InventoryComponent->ItemDroppedDelegate.AddLambda(
			[this] (const FRPGInventoryEntry* Entry, int32 NumItems)
			{
				SpawnItem(Entry, NumItems);
			});

		EquipmentComponent->EquipmentList.UnEquippedEntryDelegate.AddLambda(
			[this] (const FRPGEquipmentEntry& UnEquippedEntry)
			{
				if (IsValid(InventoryComponent))
				{
					InventoryComponent->AddUnEquippedItemEntry(UnEquippedEntry.EntryTag, UnEquippedEntry.EffectPackage);
				}
			});
	}
}

void ARPGPlayerController::SpawnItem(const FRPGInventoryEntry* Entry, int32 NumItems)
{
	if (Entry && IsValid(InventoryComponent))
	{
		const FVector ForwardLocation = GetPawn()->GetActorLocation() + GetPawn()->GetActorForwardVector() * ItemSpawnForwardDistance;
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(ForwardLocation);

		AItemActor* NewActor = GetWorld()->SpawnActorDeferred<AItemActor>(AItemActor::StaticClass(), SpawnTransform);

		NewActor->SetParams(Entry, NumItems);

		FMasterItemDefinition Item = InventoryComponent->GetItemDefinitionByTag(Entry->ItemTag);

		if (IsValid(Item.ItemMesh.Get()))
		{
			NewActor->SetMesh(Item.ItemMesh.Get());
			NewActor->FinishSpawning(SpawnTransform);
		}
		else
		{
			FStreamableManager& Manager = UAssetManager::GetStreamableManager();

			Manager.RequestAsyncLoad(Item.ItemMesh.ToSoftObjectPath(),
				[NewActor, Item, SpawnTransform]
				{
					NewActor->SetMesh(Item.ItemMesh.Get());
					NewActor->FinishSpawning(SpawnTransform);
				});
		}
	}
}

UInventoryComponent* ARPGPlayerController::GetInventoryComponent_Implementation()
{
	return InventoryComponent;
}

void ARPGPlayerController::SetDynamicProjectile_Implementation(const FGameplayTag& ProjectileTag, int32 AbilityLevel)
{
	if (IsValid(RPGAbilitySystemComp))
	{
		RPGAbilitySystemComp->SetDynamicProjectile(ProjectileTag, AbilityLevel);
	}
}

UAbilitySystemComponent* ARPGPlayerController::GetAbilitySystemComponent() const
{
	return UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
}

UInventoryWidgetController* ARPGPlayerController::GetInventoryWidgetController()
{
	if (!IsValid(InventoryWidgetController))
	{
		InventoryWidgetController = NewObject<UInventoryWidgetController>(this, InventoryWidgetControllerClass);
		InventoryWidgetController->SetOwningActor(this);
		InventoryWidgetController->BindCallbacksToDependencies();
	}

	return InventoryWidgetController;
}

void ARPGPlayerController::CreateInventoryWidget()
{
	if (UUserWidget* Widget = CreateWidget<URPGSystemsWidget>(this, InventoryWidgetClass))
	{
		InventoryWidget = Cast<URPGSystemsWidget>(Widget);
		InventoryWidget->SetWidgetController(GetInventoryWidgetController());
		InventoryWidgetController->BroadcastInitialValues();
		InventoryWidget->AddToViewport();
	}
}

