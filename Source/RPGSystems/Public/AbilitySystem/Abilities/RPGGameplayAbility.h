// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RPGGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API URPGGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category= "Custom Values|Input")
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly, Category= "Custom Values|Multiplayer")
	bool bIsClientPassive = false;
};
