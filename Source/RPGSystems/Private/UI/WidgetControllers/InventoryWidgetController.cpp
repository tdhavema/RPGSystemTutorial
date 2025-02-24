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
		OwningInventory->InventoryList.DirtyItemDelegate.AddLambda(
			[this] (const FRPGInventoryEntry& DirtyItem)
			{
				InventoryEntryDelegate.Broadcast(DirtyItem);
			});

		OwningInventory->InventoryList.ItemRemovedDelegate.AddLambda(
			[this] (int64 ItemID)
			{
				OnInventoryItemRemoved.Broadcast(ItemID);
			});
	}
}

void UInventoryWidgetController::BroadcastInitialValues()
{
	if (IsValid(OwningInventory))
	{
		for (const FRPGInventoryEntry& Entry : OwningInventory->GetInventoryEntries())
		{
			InventoryEntryDelegate.Broadcast(Entry);
		}
	}
}