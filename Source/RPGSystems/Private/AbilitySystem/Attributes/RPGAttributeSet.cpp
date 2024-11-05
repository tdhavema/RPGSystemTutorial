// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/RPGAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

void URPGAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, Shield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, MaxShield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, DamageReduction, COND_None, REPNOTIFY_Always);
}

void URPGAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	if (Data.EvaluatedData.Attribute == GetIncomingHealthDamageAttribute())
	{
		HandleIncomingHealthDamage(Data);
	}
	if (Data.EvaluatedData.Attribute == GetIncomingShieldDamageAttribute())
	{
		HandleIncomingShieldDamage(Data);
	}
}

void URPGAttributeSet::HandleIncomingHealthDamage(const FGameplayEffectModCallbackData& Data)
{
	const float LocalDamage = GetIncomingHealthDamage();
	SetIncomingHealthDamage(0.f);

	SetHealth(FMath::Clamp(GetHealth() - LocalDamage, 0.f, GetMaxHealth()));
}

void URPGAttributeSet::HandleIncomingShieldDamage(const FGameplayEffectModCallbackData& Data)
{
	const float LocalDamage = GetIncomingShieldDamage();
	SetIncomingShieldDamage(0.f);

	SetShield(FMath::Clamp(GetShield() - LocalDamage, 0.f, GetMaxShield()));
}

void URPGAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, Health, OldHealth);
}

void URPGAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, MaxHealth, OldMaxHealth);
}

void URPGAttributeSet::OnRep_Shield(const FGameplayAttributeData& OldShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, Shield, OldShield);
}

void URPGAttributeSet::OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, MaxShield, OldMaxShield);
}

void URPGAttributeSet::OnRep_DamageReduction(const FGameplayAttributeData& OldDamageReduction)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, DamageReduction, OldDamageReduction);
}

void URPGAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, Mana, OldMana);
}

void URPGAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, MaxMana, OldMaxMana);
}
