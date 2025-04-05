// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

UCLASS()
class RPGSYSTEMS_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:

	ACharacterBase(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintImplementableEvent)
	void OnHealthChanged(float CurrentHealth, float MaxHealth);

	UFUNCTION(BlueprintImplementableEvent)
	void OnManaChanged(float CurrentMana, float MaxMana);

	UFUNCTION(BlueprintImplementableEvent)
	void OnShieldChanged(float CurrentShield, float MaxShield);

protected:
	
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo();
	virtual void BindCallbacksToDependencies();
	virtual void InitClassDefaults();

	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValues();

	UPROPERTY(EditAnywhere, Category="Custom Values|Character Info")
	FGameplayTag CharacterTag;
};
