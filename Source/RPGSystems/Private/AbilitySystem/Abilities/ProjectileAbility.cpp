// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/ProjectileAbility.h"

#include "Data/ProjectileInfo.h"
#include "Libraries/RPGAbilitySystemLibrary.h"
#include "Projectiles/ProjectileBase.h"

UProjectileAbility::UProjectileAbility()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UProjectileAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	AvatarActorFromInfo = GetAvatarActorFromActorInfo();

	if (!IsValid(AvatarActorFromInfo)) return;

	if (UProjectileInfo* ProjectileInfo = URPGAbilitySystemLibrary::GetProjectileInfo(GetAvatarActorFromActorInfo()))
	{
		if (ProjectileToSpawnTag.IsValid())
		{
			CurrentProjectileParams = *ProjectileInfo->ProjectileInfoMap.Find(ProjectileToSpawnTag);
		}
	}
}

void UProjectileAbility::SpawnProjectile(const FVector& TargetLocation)
{
	if (!IsValid(AvatarActorFromInfo) || !IsValid(CurrentProjectileParams.ProjectileClass)) return;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
		FString::Printf(TEXT("Spawning Projectile Class: %s"), *CurrentProjectileParams.ProjectileClass->GetName()));
}
