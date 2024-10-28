// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/ProjectileAbility.h"
#include "Projectiles/ProjectileBase.h"
#include "Data/ProjectileInfo.h"
#include "Interfaces/RPGAbilitySystemInterface.h"
#include "Libraries/RPGAbilitySystemLibrary.h"
#include "AbilitySystem/RPGAbilityTypes.h"

UProjectileAbility::UProjectileAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UProjectileAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	AvatarActorFromInfo = GetAvatarActorFromActorInfo();
	
	if (!ProjectileToSpawnTag.IsValid() || !IsValid(AvatarActorFromInfo)) return;

	if (UProjectileInfo* ProjectileInfo = URPGAbilitySystemLibrary::GetProjectileInfo(AvatarActorFromInfo))
	{
		CurrentProjectileParams = *ProjectileInfo->ProjectileInfoMap.Find(ProjectileToSpawnTag);
	}
	
}

void UProjectileAbility::SpawnProjectile()
{
	if (!IsValid(CurrentProjectileParams.ProjectileClass)) return;

	if (const USceneComponent* SpawnPointComp = IRPGAbilitySystemInterface::Execute_GetDynamicSpawnPoint(AvatarActorFromInfo))
	{
		const FVector SpawnPoint = SpawnPointComp->GetComponentLocation();
		const FVector TargetLocation = AvatarActorFromInfo->GetActorForwardVector() * 10000;
		const FRotator TargetRotation = (TargetLocation - SpawnPoint).Rotation();

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SpawnPoint);
		SpawnTransform.SetRotation(TargetRotation.Quaternion());

		if (AProjectileBase* SpawnedProjectile = GetWorld()->SpawnActorDeferred<AProjectileBase>(CurrentProjectileParams.ProjectileClass, SpawnTransform, AvatarActorFromInfo))
		{
			SpawnedProjectile->SetProjectileParams(CurrentProjectileParams);

			FDamageEffectInfo DamageEffectInfo;
			CaptureDamageEffectInfo(nullptr, DamageEffectInfo);

			SpawnedProjectile->DamageEffectInfo = DamageEffectInfo;

			SpawnedProjectile->FinishSpawning(SpawnTransform);
		}
	}
}
