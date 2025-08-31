// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "EquipmentDefinition.generated.h"

class AEquipmentActor;
class UEquipmentInstance;
class UGameplayEffect;

USTRUCT()
struct FEquipmentActorsToSpawn
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<AEquipmentActor> EquipmentClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FName AttachName = FName();
	
};

USTRUCT(BlueprintType)
struct FPossibleEquipmentStats
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag ImplicitTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer PrefixCategoryTags = FGameplayTagContainer::EmptyContainer;
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer SpecificPrefixTags = FGameplayTagContainer::EmptyContainer;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer SuffixCategoryTags = FGameplayTagContainer::EmptyContainer;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer SpecificSuffixTags = FGameplayTagContainer::EmptyContainer;
};

USTRUCT()
struct FTaggedDamageType
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag DamageTypeTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UGameplayEffect> EffectClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	int32 EffectLevel = 1;
};
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class RPGSYSTEMS_API UEquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Info")
	FGameplayTag ItemTag;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Info")
	FGameplayTag SlotTag;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Info")
	FGameplayTag RarityTag;
	
	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Info")
	TSubclassOf<UEquipmentInstance> InstanceType;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Stats")
	int32 MinPossibleStats = 0;
	
	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Stats")
	int32 MaxPossibleStats = 1;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Stats")
	FPossibleEquipmentStats PossibleStatRolls;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Actors")
	TArray<FEquipmentActorsToSpawn> ActorsToSpawn;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Ability")
	FTaggedDamageType BaseDamage;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Ability")
	FGameplayTagContainer PossibleAbilityRolls;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Ability")
	float ProbabilityToRollAbility = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Ability")
	bool bForceAbilityRoll = false;
	
};
