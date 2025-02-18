// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "EquipmentTypes.generated.h"

class UGameplayAbility;
class UGameplayEffect;

USTRUCT()
struct FEquipmentGrantedHandles
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayAbilitySpecHandle GrantedAbility = FGameplayAbilitySpecHandle();

	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> ActiveEffects = TArray<FActiveGameplayEffectHandle>();

	void AddEffectHandle(FActiveGameplayEffectHandle EffectHandle)
	{
		ActiveEffects.Add(EffectHandle);
	}
	
};

USTRUCT(BlueprintType)
struct FEquipmentStatEffectGroup : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag StatEffectTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeName = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftClassPtr<UGameplayEffect> EffectClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinStatLevel = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxStatLevel = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bFractionalStat = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ProbabilityToSelect = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float CurrentValue = 0.f;
	
};

USTRUCT(BlueprintType)
struct FEquipmentAbilityGroup : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AbilityTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AbilityName = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftClassPtr<UGameplayAbility> AbilityClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 AbilityLevel = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag ContextTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ProbabilityToSelect = 0.f;
	
};

USTRUCT(BlueprintType)
struct FEquipmentEffectPackage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TArray<FEquipmentStatEffectGroup> StatEffects = TArray<FEquipmentStatEffectGroup>();

	UPROPERTY(BlueprintReadOnly)
	FEquipmentAbilityGroup Ability = FEquipmentAbilityGroup();
};
