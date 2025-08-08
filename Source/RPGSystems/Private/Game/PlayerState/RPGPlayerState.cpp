// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/PlayerState/RPGPlayerState.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/RPGAttributeSet.h"

ARPGPlayerState::ARPGPlayerState()
{
	SetNetUpdateFrequency(100.f);
	SetMinNetUpdateFrequency(66.f);

	RPGAbilitySystemComp = CreateDefaultSubobject<URPGAbilitySystemComponent>("AbilitySystemComp");
	RPGAbilitySystemComp->SetIsReplicated(true);
	RPGAbilitySystemComp->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	RPGAttributes = CreateDefaultSubobject<URPGAttributeSet>("AttributeSet");
}

UAbilitySystemComponent* ARPGPlayerState::GetAbilitySystemComponent() const
{
	return RPGAbilitySystemComp;
}

URPGAbilitySystemComponent* ARPGPlayerState::GetRPGAbilitySystemComponent() const
{
	return RPGAbilitySystemComp;
}

URPGAttributeSet* ARPGPlayerState::GetRPGAttributes() const
{
	return RPGAttributes;
}

void ARPGPlayerState::AddToExperience(const FScalableFloat& XPScale)
{
	CurrentExperience += XPScale.GetValueAtLevel(PlayerLevel);
	const float RequiredExperience = RequiredLevelUpExperience.GetValueAtLevel(PlayerLevel);

	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue,
		FString::Printf(TEXT("%s Gained %.2f Experience"), *GetOwner()->GetName(), XPScale.GetValueAtLevel(PlayerLevel)));

	if (CurrentExperience >= RequiredExperience)
	{
		PlayerLevel++;
		CurrentExperience = CurrentExperience - RequiredExperience;
	}
}
