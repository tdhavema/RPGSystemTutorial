// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "EquipmentDefinition.generated.h"

class UEquipmentInstance;
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
	
};
