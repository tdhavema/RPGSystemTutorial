// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "RPGAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API URPGAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet, Health); 

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth; 
	ATTRIBUTE_ACCESSORS(URPGAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Shield)
	FGameplayAttributeData Shield;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet, Shield);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxShield)
	FGameplayAttributeData MaxShield;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet, MaxShield);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_DamageReduction)
	FGameplayAttributeData DamageReduction;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet, DamageReduction);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_CritChance)
	FGameplayAttributeData CritChance;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet, CritChance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_CritDamage)
	FGameplayAttributeData CritDamage;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet, CritDamage);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Mana)
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet, Mana);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxMana)
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet, MaxMana);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MovementSpeed)
	FGameplayAttributeData MovementSpeed;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet, MovementSpeed);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_OutgoingAbilityDamage)
	FGameplayAttributeData OutgoingAbilityDamage;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet, OutgoingAbilityDamage);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_AbilityDamageResistance)
	FGameplayAttributeData AbilityDamageResistance;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet, AbilityDamageResistance);

	UPROPERTY()
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet, IncomingDamage)

private:

	void HandleIncomingDamage(const FGameplayEffectModCallbackData& Data);


	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth); 

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	void OnRep_Shield(const FGameplayAttributeData& OldShield);
	
	UFUNCTION()
	void OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield);

	UFUNCTION()
	void OnRep_DamageReduction(const FGameplayAttributeData& OldDamageReduction);

	UFUNCTION()
	void OnRep_CritChance(const FGameplayAttributeData& OldCritChance);
	
	UFUNCTION()
	void OnRep_CritDamage(const FGameplayAttributeData& OldCritDamage);

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana);
	
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);

	UFUNCTION()
	void OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed);

	UFUNCTION()
	void OnRep_OutgoingAbilityDamage(const FGameplayAttributeData& OldOutgoingAbilityDamage);

	UFUNCTION()
	void OnRep_AbilityDamageResistance(const FGameplayAttributeData& OldAbilityDamageResistance);
	
};
