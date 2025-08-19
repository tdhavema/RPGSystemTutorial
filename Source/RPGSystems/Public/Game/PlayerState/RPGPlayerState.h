// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ScalableFloat.h"
#include "GameFramework/PlayerState.h"
#include "RPGPlayerState.generated.h"

class UGameplayEffect;
class URPGAttributeSet;
class URPGAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API ARPGPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	ARPGPlayerState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintPure)
	URPGAbilitySystemComponent* GetRPGAbilitySystemComponent() const;

	UFUNCTION(BlueprintPure)
	URPGAttributeSet* GetRPGAttributes() const;

	void AddToExperience(const FScalableFloat& XPScale);

	int32 CurrentExperience = 0;
	int32 PlayerLevel = 1;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Experience")
	FScalableFloat RequiredLevelUpExperience;

private:

	void LevelUp();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<URPGAbilitySystemComponent> RPGAbilitySystemComp;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<URPGAttributeSet> RPGAttributes;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Level Up")
	TSubclassOf<UGameplayEffect> LevelUpEffect;
	
};
