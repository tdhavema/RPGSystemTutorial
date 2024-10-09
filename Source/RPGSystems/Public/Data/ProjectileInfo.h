// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/RPGAbilityTypes.h"
#include "Engine/DataAsset.h"
#include "ProjectileInfo.generated.h"

/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UProjectileInfo : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, FProjectileParams> ProjectileInfoMap;
	
};
