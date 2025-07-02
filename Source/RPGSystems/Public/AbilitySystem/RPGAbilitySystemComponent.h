// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Equipment/EquipmentManagerComponent.h"
#include "RPGAbilitySystemComponent.generated.h"

struct FRPGEquipmentEntry;
DECLARE_MULTICAST_DELEGATE(FOnAttributesGiven);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquipmentAbilityGiven, FRPGEquipmentEntry* /* Equipment Entry */);
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API URPGAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

	FOnAttributesGiven OnAttributesGiven;
	FOnEquipmentAbilityGiven OnEquipmentAbilityGiven;
	
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& AbilitiesToGrant);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& PassivesToGrant);
	void InitializeDefaultAttributes(const TSubclassOf<UGameplayEffect>& AttributeEffect);

	void AbilityInputPressed(const FGameplayTag& InputTag);
	void AbilityInputReleased(const FGameplayTag& InputTag);

	void SetDynamicProjectile(const FGameplayTag& ProjectileTag, int32 AbilityLevel = 1);

	void AddEquipmentEffects(FRPGEquipmentEntry* EquipmentEntry);
	void RemoveEquipmentEffects(FRPGEquipmentEntry* EquipmentEntry);
	void AddEquipmentAbility(FRPGEquipmentEntry* EquipmentEntry);
	void RemoveEquipmentAbility(const FRPGEquipmentEntry* EquipmentEntry);

private:

	FGameplayAbilitySpecHandle ActiveProjectileAbility;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Projectile Ability")
	TSubclassOf<UGameplayAbility> DynamicProjectileAbility;

	FGameplayAbilitySpecHandle GrantEquipmentAbility(const FRPGEquipmentEntry* EquipmentEntry);

	UFUNCTION(Server, Reliable)
	void ServerSetDynamicProjectile(const FGameplayTag& ProjectileTag, int32 AbilityLevel);
	
};
