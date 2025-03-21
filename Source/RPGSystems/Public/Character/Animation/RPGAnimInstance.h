// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Animation/AnimInstance.h"
#include "RPGAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API URPGAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);
	
private:

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Property Map")
	FGameplayTagBlueprintPropertyMap PropertyMap;
	
};
