// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "EquipmentTypes.generated.h"

USTRUCT()
struct FEquipmentGrantedHandles
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> GrantedAbilities = TArray<FGameplayAbilitySpecHandle>();

	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> ActiveEffects = TArray<FActiveGameplayEffectHandle>();

	void AddAbilityHandle(FGameplayAbilitySpecHandle AbilityHandle)
	{
		GrantedAbilities.Add(AbilityHandle);
	}

	void AddEffectHandle(FActiveGameplayEffectHandle EffectHandle)
	{
		ActiveEffects.Add(EffectHandle);
	}
	
};
