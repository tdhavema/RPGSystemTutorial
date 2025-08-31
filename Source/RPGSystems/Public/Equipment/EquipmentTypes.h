// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"
#include "EquipmentTypes.generated.h"

class UGameplayAbility;
class UGameplayEffect;

UENUM()
enum EEquipmentStatGroup
{
	ESG_None,

	Prefix,
	Suffix,
};

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
	FText StatEffectName = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag ContextTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftClassPtr<UGameplayEffect> EffectClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinStatLevel = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxStatLevel = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bFractionalStat = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsMultiplier = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ProbabilityToSelect = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float CurrentValue = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FScalableFloat NonLinearCurve = FScalableFloat();
	
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
	FGameplayTagContainer AbilityContextTags = FGameplayTagContainer::EmptyContainer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 AbilityLevel = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag ContextTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ProbabilityToSelect = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FScalableFloat MinDamageCoefficient = FScalableFloat();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FScalableFloat MaxDamageCoefficient = FScalableFloat();
	
};

USTRUCT(BlueprintType)
struct FEquipmentEffectPackage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FEquipmentStatEffectGroup BaseDamage = FEquipmentStatEffectGroup();

	UPROPERTY(BlueprintReadOnly)
	FEquipmentStatEffectGroup Implicit = FEquipmentStatEffectGroup();

	UPROPERTY(BlueprintReadOnly)
	TArray<FEquipmentStatEffectGroup> Prefixes = TArray<FEquipmentStatEffectGroup>();

	UPROPERTY(BlueprintReadOnly)
	TArray<FEquipmentStatEffectGroup> Suffixes = TArray<FEquipmentStatEffectGroup>();

	UPROPERTY(BlueprintReadOnly)
	FEquipmentAbilityGroup Ability = FEquipmentAbilityGroup();

	UPROPERTY(BlueprintReadOnly)
	int32 MaxNumPrefixes = 3;

	UPROPERTY(BlueprintReadOnly)
	int32 MaxNumSuffixes = 3;
};
