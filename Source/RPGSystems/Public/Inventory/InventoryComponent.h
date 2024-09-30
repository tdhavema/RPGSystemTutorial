// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ItemTypes.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class UItemTypesToTables;

USTRUCT()
struct FPackagedInventory
{
	GENERATED_BODY()

	virtual ~FPackagedInventory() = default;

	UPROPERTY()
	TArray<FGameplayTag> ItemTags;

	UPROPERTY()
	TArray<int32> ItemQuantities;

	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);
	
};
template<>
struct TStructOpsTypeTraits<FPackagedInventory> : TStructOpsTypeTraitsBase2<FPackagedInventory>
{
	enum
	{
		WithNetSerializer = true
	};
};

DECLARE_MULTICAST_DELEGATE_OneParam(FInventoryPackagedSignature, const FPackagedInventory& /* InventoryContents*/);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGSYSTEMS_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UInventoryComponent();

	FInventoryPackagedSignature InventoryPackagedDelegate;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	void AddItem(const FGameplayTag& ItemTag, int32 NumItems = 1);

	UFUNCTION(BlueprintCallable)
	void UseItem(const FGameplayTag& ItemTag, int32 NumItems);

	UFUNCTION(BlueprintPure)
	FMasterItemDefinition GetItemDefinitionByTag(const FGameplayTag& ItemTag) const;

	TMap<FGameplayTag, int32> GetInventoryTagMap();

	void ReconstructInventoryMap(const FPackagedInventory& Inventory);

	bool bOwnerLocallyControlled = false;

private:

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TMap<FGameplayTag, int32> InventoryTagMap;

	UPROPERTY(ReplicatedUsing=OnRep_CachedInventory)
	FPackagedInventory CachedInventory;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UItemTypesToTables> InventoryDefinitions;

	UFUNCTION(Server, Reliable)
	void ServerAddItem(const FGameplayTag& ItemTag, int32 NumItems);

	UFUNCTION(Server, Reliable)
	void ServerUseItem(const FGameplayTag& ItemTag, int32 NumItems); 

	void PackageInventory(FPackagedInventory& OutInventory);

	UFUNCTION()
	void OnRep_CachedInventory();
		
};
