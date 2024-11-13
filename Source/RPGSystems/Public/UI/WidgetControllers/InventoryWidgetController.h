// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetControllers/WidgetController.h"
#include "InventoryWidgetController.generated.h"

struct FMasterItemDefinition;
class UInventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryItemSignature, const FMasterItemDefinition&, Item);
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

	void SetOwningActor(AActor* InOwner);

	void BindCallbacksToDependencies();
	void BroadcastInitialValues();

private:

	UPROPERTY()
	TObjectPtr<AActor> OwningActor;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> OwningInventory;
};
