// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RPGAbilitySystemLibrary.generated.h"

class UAbilitySystemComponent;
struct FDamageEffectInfo;
class UProjectileInfo;
class UCharacterClassInfo;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API URPGAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure)
	static UCharacterClassInfo* GetCharacterClassDefaultInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure)
	static UProjectileInfo* GetProjectileInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
	static void ApplyDamageEffect(const FDamageEffectInfo& DamageEffectInfo);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Loose Tag Count Static"))
	static void K2_SetLooseTagCountStatic(UAbilitySystemComponent* ASC, FGameplayTag Tag, int32 NewCount);

	UFUNCTION(BlueprintPure)
	static float GetScalableFloatValueAtLevel(const FScalableFloat& Curve, float InLevel);

	template<typename T>
	static T* GetDataTableRowByTag(UDataTable* DataTable, FGameplayTag Tag);
	
};

template <typename T>
T* URPGAbilitySystemLibrary::GetDataTableRowByTag(UDataTable* DataTable, FGameplayTag Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), FString(""));
}
