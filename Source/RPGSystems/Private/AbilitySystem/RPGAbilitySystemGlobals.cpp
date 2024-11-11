// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/RPGAbilitySystemGlobals.h"

#include "AbilitySystem/RPGAbilityTypes.h"

FGameplayEffectContext* URPGAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FRPGGameplayEffectContext();
}
