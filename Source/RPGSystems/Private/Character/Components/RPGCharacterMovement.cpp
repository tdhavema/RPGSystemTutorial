// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/RPGCharacterMovement.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/RPGGameplayTags.h"

void URPGCharacterMovement::Crouch(bool bClientSimulation)
{
	Super::Crouch(bClientSimulation);

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawnOwner()))
	{
		ASC->AddLooseGameplayTag(RPGGameplayTags::Player::State::Crouch);
	}
}

void URPGCharacterMovement::UnCrouch(bool bClientSimulation)
{
	Super::UnCrouch(bClientSimulation);

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawnOwner()))
	{
		ASC->RemoveLooseGameplayTag(RPGGameplayTags::Player::State::Crouch);
	}
}
