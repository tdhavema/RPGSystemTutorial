// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/AbilityTask_InteractTrace.h"

#include "AbilitySystemComponent.h"
#include "Interfaces/InteractInterface.h"

UAbilityTask_InteractTrace::UAbilityTask_InteractTrace()
{
	bTickingTask = true;
}

UAbilityTask_InteractTrace* UAbilityTask_InteractTrace::InteractTrace(UGameplayAbility* OwningAbility)
{
	return NewAbilityTask<UAbilityTask_InteractTrace>(OwningAbility);
}

void UAbilityTask_InteractTrace::Activate()
{
	Super::Activate();

	if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
	{
		PlayerController = ASC->AbilityActorInfo->PlayerController.Get();
	}
}

void UAbilityTask_InteractTrace::TickTask(float DeltaTime)
{
	if (!IsValid(PlayerController))
	{
		EndTask();
	}
	
	Super::TickTask(DeltaTime);

	PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, InteractTraceHit);

	ThisFrameInteractActor = InteractTraceHit.GetActor();

	if (ThisFrameInteractActor)
	{
		if (ThisFrameInteractActor != LastFrameInteractActor)
		{
			IInteractInterface::Execute_BeginInteract(ThisFrameInteractActor.GetObject(), PlayerController);
		}
	}
	if (LastFrameInteractActor)
	{
		if (LastFrameInteractActor != ThisFrameInteractActor)
		{
			IInteractInterface::Execute_EndInteract(LastFrameInteractActor.GetObject());
		}
	}

	LastFrameInteractActor = ThisFrameInteractActor;
}
