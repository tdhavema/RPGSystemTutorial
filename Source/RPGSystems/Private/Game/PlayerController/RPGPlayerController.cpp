// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/PlayerController/RPGPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Game/PlayerState/RPGPlayerState.h"
#include "Input/RPGSystemsInputComponent.h"
#include "Inventory/InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "UI/WidgetControllers/InventoryWidgetController.h"
#include "UI/RPGSystemsWidget.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "Equipment/EquipmentManagerComponent.h"

ARPGPlayerController::ARPGPlayerController()
{
	bReplicates = true;

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");
	InventoryComponent->SetIsReplicated(true);

	EquipmentComponent = CreateDefaultSubobject<UEquipmentManagerComponent>("EquipmentComponent");
}

void ARPGPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (URPGSystemsInputComponent* RPGInputComp = Cast<URPGSystemsInputComponent>(InputComponent))
	{
		RPGInputComp->BindAbilityActions(RPGInputConfig, this, &ThisClass::AbilityInputPressed, &ThisClass::AbilityInputReleased);
	}
}

void ARPGPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGPlayerController, InventoryComponent);
}

void ARPGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BindCallbacksToDependencies();
}

void ARPGPlayerController::AbilityInputPressed(FGameplayTag InputTag)
{
	if (IsValid(GetRPGAbilitySystemComponent()))
	{
		RPGAbilitySystemComp->AbilityInputPressed(InputTag);
	}
}

void ARPGPlayerController::AbilityInputReleased(FGameplayTag InputTag)
{
	if (IsValid(GetRPGAbilitySystemComponent()))
	{
		RPGAbilitySystemComp->AbilityInputReleased(InputTag);
	}
}

URPGAbilitySystemComponent* ARPGPlayerController::GetRPGAbilitySystemComponent()
{
	if (!IsValid(RPGAbilitySystemComp))
	{
		if (const ARPGPlayerState* RPGPlayerState = GetPlayerState<ARPGPlayerState>())
		{
			RPGAbilitySystemComp = RPGPlayerState->GetRPGAbilitySystemComponent();
		}
	}

	return RPGAbilitySystemComp;
}

void ARPGPlayerController::BindCallbacksToDependencies()
{
	if (IsValid(InventoryComponent) && IsValid(EquipmentComponent))
	{
		InventoryComponent->EquipmentItemDelegate.AddLambda(
			[this] (const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition, const TArray<FEquipmentStatEffectGroup>& StatEffects)
			{
				if (IsValid(EquipmentComponent))
				{
					EquipmentComponent->EquipItem(EquipmentDefinition, StatEffects);
				}
			});

		EquipmentComponent->EquipmentList.UnEquippedEntryDelegate.AddLambda(
			[this] (const FRPGEquipmentEntry& UnEquippedEntry)
			{
				if (IsValid(InventoryComponent))
				{
					InventoryComponent->AddUnEquippedItemEntry(UnEquippedEntry.EntryTag, UnEquippedEntry.StatEffects);
				}
			});
	}
}

UInventoryComponent* ARPGPlayerController::GetInventoryComponent_Implementation()
{
	return InventoryComponent;
}

void ARPGPlayerController::SetDynamicProjectile_Implementation(const FGameplayTag& ProjectileTag, int32 AbilityLevel)
{
	if (IsValid(RPGAbilitySystemComp))
	{
		RPGAbilitySystemComp->SetDynamicProjectile(ProjectileTag, AbilityLevel);
	}
}

UAbilitySystemComponent* ARPGPlayerController::GetAbilitySystemComponent() const
{
	return UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
}

UInventoryWidgetController* ARPGPlayerController::GetInventoryWidgetController()
{
	if (!IsValid(InventoryWidgetController))
	{
		InventoryWidgetController = NewObject<UInventoryWidgetController>(this, InventoryWidgetControllerClass);
		InventoryWidgetController->SetOwningActor(this);
		InventoryWidgetController->BindCallbacksToDependencies();
	}

	return InventoryWidgetController;
}

void ARPGPlayerController::CreateInventoryWidget()
{
	if (UUserWidget* Widget = CreateWidget<URPGSystemsWidget>(this, InventoryWidgetClass))
	{
		InventoryWidget = Cast<URPGSystemsWidget>(Widget);
		InventoryWidget->SetWidgetController(GetInventoryWidgetController());
		InventoryWidgetController->BroadcastInitialValues();
		InventoryWidget->AddToViewport();
	}
}

