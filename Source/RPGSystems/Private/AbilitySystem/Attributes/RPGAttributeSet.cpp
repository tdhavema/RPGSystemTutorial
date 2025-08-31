// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/RPGAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "AbilitySystem/RPGAbilityTypes.h"
#include "Character/EnemyBase.h"
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
	DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, CritChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, CritDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, OutgoingAbilityDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, AbilityDamageResistance, COND_None, REPNOTIFY_Always);
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
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		HandleIncomingDamage(Data);
	}
}

void URPGAttributeSet::HandleIncomingDamage(const FGameplayEffectModCallbackData& Data)
{
	const float LocalDamage = GetIncomingDamage();
	SetIncomingDamage(0.f);

	const float LocalShield = GetShield();
	float OutShield = 0.f;

	if (LocalDamage > 0.f && LocalShield > 0.f)
	{
		OutShield = LocalShield - LocalDamage;
		SetShield(FMath::Clamp(OutShield, 0.f, GetMaxShield()));
	}

	if (LocalDamage > 0.f && OutShield <= 0.f)
	{
		const float RemainderDamage = fabs(LocalShield - LocalDamage);
		SetHealth(FMath::Clamp(GetHealth() - RemainderDamage, 0.f, GetMaxHealth()));
	}

	FGameplayEffectContextHandle ContextHandle = Data.EffectSpec.GetContext();
	FRPGGameplayEffectContext* RPGContext = FRPGGameplayEffectContext::GetEffectContext(ContextHandle);
	FColor DebugColor = RPGContext->IsCriticalHit() ? FColor::Red : FColor::Green;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, DebugColor,
		FString::Printf(TEXT("Damage Dealt %.2f"), LocalDamage));

	AActor* TargetActor = Data.Target.GetAvatarActor();

	if (IsValid(TargetActor) && TargetActor->IsA(AEnemyBase::StaticClass()) && TargetActor->HasAuthority())
	{
		if (UAbilitySystemComponent* AttackingASC = RPGContext->GetOriginalInstigatorAbilitySystemComponent())
		{
			IRPGAbilitySystemInterface::Execute_AddAttackingActor(TargetActor, AttackingASC->GetAvatarActor());
		}
	}

	if (GetHealth() < 1.f && IsValid(TargetActor))
	{
		IRPGAbilitySystemInterface::Execute_Death(TargetActor);
	}
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

void URPGAttributeSet::OnRep_CritChance(const FGameplayAttributeData& OldCritChance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, CritChance, OldCritChance);
}

void URPGAttributeSet::OnRep_CritDamage(const FGameplayAttributeData& OldCritDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, CritDamage, OldCritDamage);
}

void URPGAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, Mana, OldMana);
}

void URPGAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, MaxMana, OldMaxMana);
}

void URPGAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, MovementSpeed, OldMovementSpeed);
}

void URPGAttributeSet::OnRep_OutgoingAbilityDamage(const FGameplayAttributeData& OldOutgoingAbilityDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, OutgoingAbilityDamage, OldOutgoingAbilityDamage);
}

void URPGAttributeSet::OnRep_AbilityDamageResistance(const FGameplayAttributeData& OldAbilityDamageResistance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, AbilityDamageResistance, OldAbilityDamageResistance);
}
