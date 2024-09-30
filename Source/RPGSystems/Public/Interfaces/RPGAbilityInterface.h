// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "RPGAbilityInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URPGAbilityInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGSYSTEMS_API IRPGAbilityInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetDynamicProjectile(const FGameplayTag& ProjectileTag);
};
