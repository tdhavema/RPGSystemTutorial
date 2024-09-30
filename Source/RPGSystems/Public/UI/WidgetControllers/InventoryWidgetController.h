// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetControllers/WidgetController.h"
#include "InventoryWidgetController.generated.h"

struct FMasterItemDefinition;
class UInventoryComponent;
struct FPackagedInventory;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryItemSignature, const FMasterItemDefinition&, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryBroadcastComplete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FScrollBoxResetSignature);
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class RPGSYSTEMS_API UInventoryWidgetController : public UWidgetController
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FInventoryItemSignature InventoryItemDelegate;

	UPROPERTY(BlueprintAssignable)
	FInventoryBroadcastComplete InventoryBroadcastComplete;

	UPROPERTY(BlueprintAssignable)
	FScrollBoxResetSignature ScrollBoxResetDelegate;

	void SetOwningActor(AActor* InOwner);

	void BindCallbacksToDependencies();
	void BroadcastInitialValues();

private:

	void UpdateInventory(const FPackagedInventory& InventoryContents);
	void BroadcastInventoryContents();

	UPROPERTY()
	TObjectPtr<AActor> OwningActor;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> OwningInventory;
};
