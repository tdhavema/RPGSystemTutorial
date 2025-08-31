// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/RPGDamageAbility.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystem/RPGAbilityTypes.h"

void URPGDamageAbility::CaptureDamageEffectInfo(AActor* TargetActor, FDamageEffectInfo& OutInfo)
{
	if (AActor* AvatarActorFromInfo = GetAvatarActorFromActorInfo())
	{
		OutInfo.AbilityLevel = GetAbilityLevel();
		OutInfo.AvatarActor = AvatarActorFromInfo;
		OutInfo.BaseDamage = FMath::FRandRange(MinDamageCoefficient.GetValueAtLevel(OutInfo.AbilityLevel),
												MaxDamageCoefficient.GetValueAtLevel(OutInfo.AbilityLevel));
		OutInfo.DamageEffect = DamageEffect;
		OutInfo.SourceASC = GetAbilitySystemComponentFromActorInfo();
		OutInfo.AbilityDynamicTags = GetCurrentAbilitySpec()->GetDynamicSpecSourceTags();

		if (IsValid(TargetActor))
		{
			OutInfo.TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
		}
	}
}
