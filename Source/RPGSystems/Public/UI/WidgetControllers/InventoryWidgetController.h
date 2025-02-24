// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetControllers/WidgetController.h"
#include "InventoryWidgetController.generated.h"

struct FRPGInventoryEntry;
class UInventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryEntrySignature, const FRPGInventoryEntry&, Entry);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryItemRemoved, const int64, ItemID);
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class RPGSYSTEMS_API UInventoryWidgetController : public UWidgetController
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FInventoryEntrySignature InventoryEntryDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnInventoryItemRemoved OnInventoryItemRemoved;

	void SetOwningActor(AActor* InOwner);

	void BindCallbacksToDependencies();
	void BroadcastInitialValues();

private:

	UPROPERTY()
	TObjectPtr<AActor> OwningActor;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> OwningInventory;
};
