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
				FMasterItemDefinition Item = OwningInventory->GetItemDefinitionByTag(DirtyItem.ItemTag);
				Item.ItemQuantity = DirtyItem.Quantity;

				InventoryItemDelegate.Broadcast(Item);
			});
	}
}

void UInventoryWidgetController::BroadcastInitialValues()
{
	if (IsValid(OwningInventory))
	{
		for (const FRPGInventoryEntry& Entry : OwningInventory->GetInventoryEntries())
		{
			FMasterItemDefinition Item = OwningInventory->GetItemDefinitionByTag(Entry.ItemTag);
			Item.ItemQuantity = Entry.Quantity;

			InventoryItemDelegate.Broadcast(Item);
		}
	}
}