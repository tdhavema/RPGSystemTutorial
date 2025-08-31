// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RPGGameplayAbility.h"
#include "RPGDamageAbility.generated.h"

struct FDamageEffectInfo;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API URPGDamageAbility : public URPGGameplayAbility
{
	GENERATED_BODY()

public:

	void CaptureDamageEffectInfo(AActor* TargetActor, FDamageEffectInfo& OutInfo);

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Damage")
	FScalableFloat MinDamageCoefficient;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Damage")
	FScalableFloat MaxDamageCoefficient;

private:

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Damage Effect")
	TSubclassOf<UGameplayEffect> DamageEffect;
};
