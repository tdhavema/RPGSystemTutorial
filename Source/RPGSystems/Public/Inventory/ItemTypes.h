// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ItemTypes.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct FConsumableProps
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> ItemEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ItemEffectLevel;
	
};

USTRUCT(BlueprintType)
struct FMasterItemDefinition : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag ItemTag;

	UPROPERTY(BlueprintReadOnly)
	int32 ItemQuantity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText ItemName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FConsumableProps ConsumableProps;
	
};
