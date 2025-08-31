// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/RPGAbilityTypes.h"
#include "AbilitySystem/RPGGameplayTags.h"
#include "AbilitySystem/Attributes/RPGAttributeSet.h"
#include "Interfaces/RPGAbilitySystemInterface.h"
#include "Libraries/RPGAbilitySystemLibrary.h"

struct RPGDamageStatics
{
	// Source Captures
	DECLARE_ATTRIBUTE_CAPTUREDEF(CritChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CritDamage);

	// Target Captures
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncomingDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageReduction);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Shield);
	
	RPGDamageStatics()
	{
		// Source Defines
		DEFINE_ATTRIBUTE_CAPTUREDEF(URPGAttributeSet, CritChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URPGAttributeSet, CritDamage, Source, false);

		// Target Defines
		DEFINE_ATTRIBUTE_CAPTUREDEF(URPGAttributeSet, IncomingDamage, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URPGAttributeSet, DamageReduction, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URPGAttributeSet, Shield, Target, false);
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
	RelevantAttributesToCapture.Add(DamageStatics().CritChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CritDamageDef);

	// Target Captures
	RelevantAttributesToCapture.Add(DamageStatics().IncomingDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().DamageReductionDef);
	RelevantAttributesToCapture.Add(DamageStatics().ShieldDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();

	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	FAggregatorEvaluateParameters EvalParams;
	EvalParams.TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();
	EvalParams.SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();

	const FGameplayEffectContextHandle EffectContextHandle = EffectSpec.GetContext();
	FRPGGameplayEffectContext* RPGContext = FRPGGameplayEffectContext::GetEffectContext(EffectContextHandle);

	// Source Captures
	float CritChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritChanceDef, EvalParams, CritChance);
	CritChance = FMath::Max<float>(CritChance, 0.f);

	float CritDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritDamageDef, EvalParams, CritDamage);
	CritDamage = FMath::Max<float>(CritDamage, 0.f);

	// Target Captures
	float Shield = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ShieldDef, EvalParams, Shield);
	Shield = FMath::Max<float>(Shield, 0.f);

	float DamageReduction = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageReductionDef, EvalParams, DamageReduction);
	DamageReduction = FMath::Max<float>(DamageReduction, 0.f);

	// Begin Calculation
	
	float Damage = 0.f;

	float DamageCoefficient = EffectSpec.GetSetByCallerMagnitude(RPGGameplayTags::Combat::Data_Damage);
	DamageCoefficient = FMath::Max<float>(DamageCoefficient, 0.f);
	
	FGameplayTagContainer DamageTypesEvaluationTags = EffectSpec.GetDynamicAssetTags();
	DamageTypesEvaluationTags.AppendTags(*EvalParams.SourceTags);

	for (const FGameplayTag& DamageTypeTag : URPGAbilitySystemLibrary::GetDamageTypeTags())
	{
		bool bOutSuccess = false;

		DamageTypesEvaluationTags.AddTag(DamageTypeTag);

		const float DamageValue = UAbilitySystemBlueprintLibrary::EvaluateAttributeValueWithTags(SourceASC, URPGAttributeSet::GetOutgoingAbilityDamageAttribute(),
			DamageTypesEvaluationTags, *EvalParams.TargetTags, bOutSuccess);

		if (DamageValue > 0.f)
		{
			const float ResistanceValue = UAbilitySystemBlueprintLibrary::EvaluateAttributeValueWithTags(TargetASC, URPGAttributeSet::GetAbilityDamageResistanceAttribute(),
				DamageTypesEvaluationTags, *EvalParams.TargetTags, bOutSuccess);

			float TotalDamage = DamageValue * DamageCoefficient;
			TotalDamage *= (100 - ResistanceValue) / 100;

			Damage += TotalDamage;

			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan,
				FString::Printf(TEXT("Damage Type %s, Damage Dealt %.2f"), *DamageTypeTag.ToString(), TotalDamage));
		}

		DamageTypesEvaluationTags.RemoveTag(DamageTypeTag);
	}
	
	const bool bCriticalHit = FMath::RandRange(0, 100) < CritChance;
	Damage = bCriticalHit ? Damage * (1 + (CritDamage / 100)) : Damage;
	RPGContext->SetIsCriticalHit(bCriticalHit);

	if (Damage > 0.f && Shield > 0.f)
	{
		Damage *= (100 - DamageReduction) / 100;
	}

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().IncomingDamageProperty, EGameplayModOp::Additive, Damage));
}
