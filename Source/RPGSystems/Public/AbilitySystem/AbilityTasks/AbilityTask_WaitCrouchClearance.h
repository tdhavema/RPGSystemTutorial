// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitCrouchClearance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHasCrouchClearance);

/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UAbilityTask_WaitCrouchClearance : public UAbilityTask
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FHasCrouchClearance HasClearance;

	UAbilityTask_WaitCrouchClearance();
	virtual void TickTask(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "OwningAbility", HidePin = "OwningAbility", BlueprintInternalUseOnly = true))
	static UAbilityTask_WaitCrouchClearance* WaitCrouchClearance(UGameplayAbility* OwningAbility, float TargetHeightOffset, float SphereTraceRadius,
		bool bShowDebug);

private:

	UPROPERTY()
	TObjectPtr<AActor> AvatarActor;

	FHitResult TraceHit;
	float TargetHeightOffset = 0.f;
	float SphereTraceRadius = 30.f;
	bool bShowDebug = false;
	
};
