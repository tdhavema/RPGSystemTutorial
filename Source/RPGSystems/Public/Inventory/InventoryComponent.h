// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ItemTypes.h"
#include "Components/ActorComponent.h"
#include "Equipment/EquipmentDefinition.h"
#include "Equipment/EquipmentTypes.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "InventoryComponent.generated.h"

class AItemActor;
class UInventoryComponent;
class UEquipmentStatEffects;
class UItemTypesToTables;

DECLARE_MULTICAST_DELEGATE_TwoParams(FEquipmentItemUsed, const TSubclassOf<UEquipmentDefinition>& /* Equipment Definition */, const FEquipmentEffectPackage& /* Stat Effects */);

USTRUCT(BlueprintType)
struct FRPGInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag ItemTag = FGameplayTag::EmptyTag;

	UPROPERTY(BlueprintReadWrite)
	FText ItemName = FText();

	UPROPERTY(BlueprintReadOnly)
	int32 Quantity = 0;

	UPROPERTY(BlueprintReadOnly)
	int64 ItemID = 0;

	UPROPERTY(BlueprintReadWrite)
	FEquipmentEffectPackage EffectPackage = FEquipmentEffectPackage();

	bool IsValid() const
	{
		return ItemID != 0;
	}

	bool operator==(const FRPGInventoryEntry& Other) const
	{
		return Other.ItemID == ItemID;
	}
	
};

DECLARE_MULTICAST_DELEGATE_OneParam(FDirtyInventoryItemSignature, const FRPGInventoryEntry& /* Dirty Item */);
DECLARE_MULTICAST_DELEGATE_OneParam(FInventoryItemRemovedSignature, const int64 /* Item ID */);

USTRUCT()
struct FRPGInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

	FRPGInventoryList() :
	OwnerComponent(nullptr)
	{}

	FRPGInventoryList(UInventoryComponent* InComponent) :
	OwnerComponent(InComponent)
	{}

	FRPGInventoryEntry* AddItem(const FGameplayTag& ItemTag, int32 NumItems = 1);
	void RemoveItem(const FRPGInventoryEntry& Entry, int32 NumItems = 1);
	bool HasEnough(const FGameplayTag& ItemTag, int32 NumItems = 1);
	uint64 GenerateID();
	void SetStats(UEquipmentStatEffects* InStats);
	void RollForStats(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition, FRPGInventoryEntry* Entry);
	void AddUnEquippedItem(const FGameplayTag& ItemTag, const FEquipmentEffectPackage& EffectPackage, int32 NumItems = 1);

	// FFastArraySerializer Contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FRPGInventoryEntry, FRPGInventoryList>(Entries, DeltaParms, *this);
	}

	FDirtyInventoryItemSignature DirtyItemDelegate;
	FInventoryItemRemovedSignature InventoryItemRemovedDelegate;

private:

	friend UInventoryComponent;

	UPROPERTY()
	TArray<FRPGInventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UInventoryComponent> OwnerComponent;

	UPROPERTY(NotReplicated)
	uint64 LastAssignedID = 0;

	UPROPERTY(NotReplicated)
	TWeakObjectPtr<UEquipmentStatEffects> WeakStats;
};

template<>
struct TStructOpsTypeTraits<FRPGInventoryList> : TStructOpsTypeTraitsBase2<FRPGInventoryList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FItemDroppedSignature, const FRPGInventoryEntry* /* Entry */, int32 /* Num Items */);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGSYSTEMS_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	FEquipmentItemUsed EquipmentItemDelegate;
	FItemDroppedSignature ItemDroppedDelegate;

	UPROPERTY(Replicated)
	FRPGInventoryList InventoryList;

	UInventoryComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void AddItem(const FGameplayTag& ItemTag, int32 NumItems = 1);

	UFUNCTION(BlueprintCallable)
	void UseItem(const FRPGInventoryEntry& Entry, int32 NumItems);

	UFUNCTION(BlueprintCallable)
	void DropItem(const FRPGInventoryEntry& Entry, int32 NumItems);

	UFUNCTION(BlueprintCallable)
	void PickupItem(AItemActor* Item);

	UFUNCTION(BlueprintPure)
	FMasterItemDefinition GetItemDefinitionByTag(const FGameplayTag& ItemTag) const;

	TArray<FRPGInventoryEntry> GetInventoryEntries();
	void AddUnEquippedItemEntry(const FGameplayTag& ItemTag, const FEquipmentEffectPackage& EffectPackage);

	UFUNCTION(BlueprintPure)
	TArray<FRPGInventoryEntry> GetEntriesByString(const FString& String);
	void SpawnItem(const FTransform& SpawnTransform, const FRPGInventoryEntry* Entry, int32 NumItems);

private:

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Stat Effects")
	TObjectPtr<UEquipmentStatEffects> StatEffects;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Item Definitions")
	TObjectPtr<UItemTypesToTables> InventoryDefinitions;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Item Spawn")
	TSubclassOf<AItemActor> DefaultItemClass;

	UFUNCTION(Server, Reliable)
	void ServerAddItem(const FGameplayTag& ItemTag, int32 NumItems);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUseItem(const FRPGInventoryEntry& Entry, int32 NumItems);

	bool ServerUseItem_Validate(const FRPGInventoryEntry& Entry, int32 NumItems);

	UFUNCTION(Server, Reliable)
	void ServerDropItem(const FRPGInventoryEntry& Entry, int32 NumItems);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPickupItem(AItemActor* Item);

	bool ServerPickupItem_Validate(AItemActor* Item);

};
