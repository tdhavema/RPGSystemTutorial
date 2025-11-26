// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/RPGDamageAbility.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystem/RPGAbilityTypes.h"


void URPGDamageAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (URPGDamageAbility* CDO = Cast<URPGDamageAbility>(Spec.Ability))
	{
		MinDamageCoefficient = CDO->MinDamageCoefficient;
		MaxDamageCoefficient = CDO->MaxDamageCoefficient;
	}
}

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
