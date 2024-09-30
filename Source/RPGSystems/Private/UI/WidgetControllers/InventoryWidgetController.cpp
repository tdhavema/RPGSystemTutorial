// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetControllers/InventoryWidgetController.h"
#include "Inventory/InventoryComponent.h"
#include "Interfaces/InventoryInterface.h"

void UInventoryWidgetController::SetOwningActor(AActor* InOwner)
{
	OwningActor = InOwner;
}

void UInventoryWidgetController::BindCallbacksToDependencies()
{
	OwningInventory = IInventoryInterface::Execute_GetInventoryComponent(OwningActor);

	if (IsValid(OwningInventory))
	{
		OwningInventory->InventoryPackagedDelegate.AddLambda(
			[this] (const FPackagedInventory& InventoryContents)
			{
				UpdateInventory(InventoryContents);
			});
	}
}

void UInventoryWidgetController::BroadcastInitialValues()
{
	if (IsValid(OwningInventory))
	{
		BroadcastInventoryContents();
	}
}

void UInventoryWidgetController::UpdateInventory(const FPackagedInventory& InventoryContents)
{
	if (IsValid(OwningInventory))
	{
		OwningInventory->ReconstructInventoryMap(InventoryContents);

		BroadcastInventoryContents();
	}
}

void UInventoryWidgetController::BroadcastInventoryContents()
{
	if (IsValid(OwningInventory))
	{
		TMap<FGameplayTag, int32> LocalInventoryMap = OwningInventory->GetInventoryTagMap();

		ScrollBoxResetDelegate.Broadcast();

		for (const auto& Pair : LocalInventoryMap)
		{
			FMasterItemDefinition Item = OwningInventory->GetItemDefinitionByTag(Pair.Key);
			Item.ItemQuantity = Pair.Value;
			InventoryItemDelegate.Broadcast(Item);
		}

		InventoryBroadcastComplete.Broadcast();
	}
}
