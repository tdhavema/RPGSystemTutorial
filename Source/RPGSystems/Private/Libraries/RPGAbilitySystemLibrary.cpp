// Fill out your copyright notice in the Description page of Project Settings.


#include "Libraries/RPGAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystem/RPGAbilityTypes.h"
#include "AbilitySystem/RPGGameplayTags.h"
#include "Game/GameMode/RPGGameMode.h"
#include "Kismet/GameplayStatics.h"

UCharacterClassInfo* URPGAbilitySystemLibrary::GetCharacterClassDefaultInfo(const UObject* WorldContextObject)
{
	if (const ARPGGameMode* RPGGameMode = Cast<ARPGGameMode>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		return RPGGameMode->GetCharacterClassDefaultInfo();
	}

	return nullptr;
}

UProjectileInfo* URPGAbilitySystemLibrary::GetProjectileInfo(const UObject* WorldContextObject)
{
	if (const ARPGGameMode* RPGGameMode = Cast<ARPGGameMode>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		return RPGGameMode->GetProjectileInfo();
	}

	return nullptr;
}

void URPGAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectInfo& DamageEffectInfo)
{
	FGameplayEffectContextHandle ContextHandle = DamageEffectInfo.SourceASC->MakeEffectContext();
	ContextHandle.AddSourceObject(DamageEffectInfo.AvatarActor);

	const FGameplayEffectSpecHandle SpecHandle = DamageEffectInfo.SourceASC->MakeOutgoingSpec(DamageEffectInfo.DamageEffect, DamageEffectInfo.AbilityLevel, ContextHandle);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, RPGGameplayTags::Combat::Data_Damage, DamageEffectInfo.BaseDamage);
	if (IsValid(DamageEffectInfo.TargetASC))
	{
		DamageEffectInfo.TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void URPGAbilitySystemLibrary::K2_SetLooseTagCountStatic(UAbilitySystemComponent* ASC, FGameplayTag Tag, int32 NewCount)
{
	if (IsValid(ASC))
	{
		ASC->SetLooseGameplayTagCount(Tag, NewCount);
	}
}

float URPGAbilitySystemLibrary::GetScalableFloatValueAtLevel(const FScalableFloat& Curve, float InLevel)
{
	return Curve.GetValueAtLevel(InLevel);
}

FGameplayTag URPGAbilitySystemLibrary::GetRandomTagFromContainer(const FGameplayTagContainer& Container)
{
	const int32 RandomIndex = FMath::RandRange(0, Container.Num() - 1);
	return Container.GetByIndex(RandomIndex);
}

FGameplayTagContainer URPGAbilitySystemLibrary::GetAllChildrenTagsOfCategories(
	const FGameplayTagContainer& CategoryTags)
{
	FGameplayTagContainer AllChildrenTags;
	const UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();

	for (const FGameplayTag& CategoryTag : CategoryTags)
	{
		AllChildrenTags.AppendTags(TagsManager.RequestGameplayTagChildren(CategoryTag));
	}

	return AllChildrenTags;
}
