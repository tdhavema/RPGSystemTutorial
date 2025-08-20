// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ItemTypes.generated.h"

class UEquipmentDefinition;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FEquipmentItemProps
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UEquipmentDefinition> EquipmentClass = nullptr;
	
};

USTRUCT(BlueprintType)
struct FConsumableProps
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> ItemEffectClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ItemEffectLevel = 1.f;
};

USTRUCT(BlueprintType)
struct FMasterItemDefinition : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag ItemTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText ItemDescription = FText();

	UPROPERTY(BlueprintReadOnly)
	int32 ItemQuantity = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText ItemName = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UStaticMesh> ItemMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Icon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FConsumableProps ConsumableProps = FConsumableProps();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FEquipmentItemProps EquipmentItemProps = FEquipmentItemProps();
	
};

USTRUCT(BlueprintType)
struct FPossibleLootParams : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag ResultingItemTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditDefaultsOnly)
	int32 MinNumItems = 1;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxNumItems = 1;

	UPROPERTY(EditDefaultsOnly)
	float ProbabilityToSelect = 0.f;
	
};
