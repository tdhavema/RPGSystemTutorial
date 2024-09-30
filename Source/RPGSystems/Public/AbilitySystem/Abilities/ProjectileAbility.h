// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/RPGAbilitySystemTypes.h"
#include "AbilitySystem/Abilities/RPGGameplayAbility.h"
#include "ProjectileAbility.generated.h"

/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UProjectileAbility : public URPGGameplayAbility
{
	GENERATED_BODY()

public:

	UProjectileAbility();

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Projectile")
	FGameplayTag ProjectileToSpawnTag;

protected:

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

private:

	UFUNCTION(BlueprintCallable)
	void SpawnProjectile(const FVector& TargetLocation);

	UPROPERTY()
	TObjectPtr<AActor> AvatarActorFromInfo;
	
	FProjectileParams CurrentProjectileParams;
	
};
