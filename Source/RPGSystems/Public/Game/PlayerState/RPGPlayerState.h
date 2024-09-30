// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "RPGPlayerState.generated.h"

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

private:

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<URPGAbilitySystemComponent> RPGAbilitySystemComp;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<URPGAttributeSet> RPGAttributes;
	
	
};
