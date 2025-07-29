// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_InteractTrace.generated.h"

class IInteractInterface;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UAbilityTask_InteractTrace : public UAbilityTask
{
	GENERATED_BODY()

public:

	UAbilityTask_InteractTrace();

	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "OwningAbility", HidePin = "OwningAbility", BlueprintInternalUseOnly = true))
	static UAbilityTask_InteractTrace* InteractTrace(UGameplayAbility* OwningAbility);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

private:

	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

	FHitResult InteractTraceHit;

	TScriptInterface<IInteractInterface> ThisFrameInteractActor;
	TScriptInterface<IInteractInterface> LastFrameInteractActor;
};
