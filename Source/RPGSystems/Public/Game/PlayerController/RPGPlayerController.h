// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/InventoryInterface.h"
#include "Interfaces/RPGAbilitySystemInterface.h"
#include "RPGPlayerController.generated.h"

struct FRPGInventoryEntry;
class UEquipmentManagerComponent;
class URPGAbilitySystemComponent;
class URPGInputConfig;
class URPGSystemsWidget;
class UInventoryWidgetController;
class UInventoryComponent;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API ARPGPlayerController : public APlayerController, public IAbilitySystemInterface, public IInventoryInterface, public IRPGAbilitySystemInterface
{
	GENERATED_BODY()

public:

	ARPGPlayerController();

	virtual void SetupInputComponent() override;

	/* Implement Inventory Interface */
	virtual UInventoryComponent* GetInventoryComponent_Implementation() override;

	/* Implement RPGAbilitySystemInterface */
	virtual void SetDynamicProjectile_Implementation(const FGameplayTag& ProjectileTag, int32 AbilityLevel) override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UInventoryWidgetController* GetInventoryWidgetController();

	UFUNCTION(BlueprintCallable)
	void CreateInventoryWidget();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	virtual void BeginPlay() override;
	void AbilityInputPressed(FGameplayTag InputTag);
	void AbilityInputReleased(FGameplayTag InputTag);

private:

	UPROPERTY()
	TObjectPtr<URPGAbilitySystemComponent> RPGAbilitySystemComp;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Input")
	TObjectPtr<URPGInputConfig> RPGInputConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Replicated)
	TObjectPtr<UInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UEquipmentManagerComponent> EquipmentComponent;

	UPROPERTY()
	TObjectPtr<UInventoryWidgetController> InventoryWidgetController;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Widgets")
	TSubclassOf<UInventoryWidgetController> InventoryWidgetControllerClass;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<URPGSystemsWidget> InventoryWidget;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Widgets")
	TSubclassOf<URPGSystemsWidget> InventoryWidgetClass;

	URPGAbilitySystemComponent* GetRPGAbilitySystemComponent();
	void BindCallbacksToDependencies();

	void SpawnItem(const FRPGInventoryEntry* Entry, int32 NumItems);

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Item Spawn")
	float ItemSpawnForwardDistance = 250.f;

};
