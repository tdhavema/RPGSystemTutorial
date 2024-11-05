// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystem/RPGGameplayTags.h"
#include "AbilitySystem/Attributes/RPGAttributeSet.h"

struct RPGDamageStatics
{
	// Source Captures

	// Target Captures
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncomingHealthDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncomingShieldDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageReduction);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Shield);
	
	RPGDamageStatics()
	{
		// Source Defines

		// Target Defines
		DEFINE_ATTRIBUTE_CAPTUREDEF(URPGAttributeSet, IncomingHealthDamage, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URPGAttributeSet, IncomingShieldDamage, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URPGAttributeSet, DamageReduction, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URPGAttributeSet, Shield, Target, false)
	}
};

static const RPGDamageStatics& DamageStatics()
{
	static RPGDamageStatics DStatics;
	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	// Source Captures

	// Target Captures
	RelevantAttributesToCapture.Add(DamageStatics().IncomingHealthDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().IncomingShieldDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().DamageReductionDef);
	RelevantAttributesToCapture.Add(DamageStatics().ShieldDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();
	const FGameplayTagContainer* SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvalParams;
	EvalParams.TargetTags = TargetTags;
	EvalParams.SourceTags = SourceTags;

	// Get raw damage value
	float Damage = EffectSpec.GetSetByCallerMagnitude(RPGGameplayTags::Combat::Data_Damage);
	Damage = FMath::Max<float>(Damage, 0.f);

	// Target Captures
	float Shield = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ShieldDef, EvalParams, Shield);
	Shield = FMath::Max<float>(Shield, 0.f);

	float DamageReduction = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageReductionDef, EvalParams, DamageReduction);
	DamageReduction = FMath::Max<float>(DamageReduction, 0.f);

	float OutShield = 0.f;

	if (Damage > 0.f && Shield > 0.f)
	{
		Damage *= (100 - DamageReduction) / 100;
		OutShield = Shield - Damage;

		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().IncomingShieldDamageProperty, EGameplayModOp::Additive, Damage));
	}

	if (OutShield <= 0.f)
	{
		const float RemainderDamage = fabs(Shield - Damage);
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().IncomingHealthDamageProperty, EGameplayModOp::Additive, RemainderDamage));
	}
}
