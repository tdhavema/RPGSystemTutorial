// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipmentTypes.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "EquipmentManagerComponent.generated.h"

class UEquipmentInstance;
class UEquipmentDefinition;
class UEquipmentManagerComponent;

USTRUCT(BlueprintType)
struct FRPGEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag EntryTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag SlotTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag RarityTag = FGameplayTag();

	UPROPERTY(NotReplicated)
	FEquipmentGrantedHandles GrantedHandles = FEquipmentGrantedHandles();

private:

	friend UEquipmentManagerComponent;
	friend struct FRPGEquipmentList;

	UPROPERTY()
	TSubclassOf<UEquipmentDefinition> EquipmentDefinition = nullptr;

	UPROPERTY()
	TObjectPtr<UEquipmentInstance> Instance = nullptr;
	
};

DECLARE_MULTICAST_DELEGATE_OneParam(FEquipmentEntrySignature, const FRPGEquipmentEntry& /* Equipment Entry */);

USTRUCT()
struct FRPGEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

	FRPGEquipmentList() :
	OwnerComponent(nullptr)
	{}

	FRPGEquipmentList(UActorComponent* InComponent) :
	OwnerComponent(InComponent)
	{}

	UEquipmentInstance* AddEntry(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition);
	void RemoveEntry(UEquipmentInstance* EquipmentInstance);

	// FFastArraySerializer Contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FRPGEquipmentEntry, FRPGEquipmentList>(Entries, DeltaParms, *this);
	}

	FEquipmentEntrySignature EquipmentEntryDelegate;
	
private:

	UPROPERTY()
	TArray<FRPGEquipmentEntry> Entries;

	UPROPERTY()
	TObjectPtr<UActorComponent> OwnerComponent;
	
};

template<>
struct TStructOpsTypeTraits<FRPGEquipmentList> : TStructOpsTypeTraitsBase2<FRPGEquipmentList>
{
	enum  { WithNetDeltaSerializer = true };
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGSYSTEMS_API UEquipmentManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(Replicated)
	FRPGEquipmentList EquipmentList;
	
	UEquipmentManagerComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipItem(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition);
	void UnEquipItem(UEquipmentInstance* EquipmentInstance);

private:

	UFUNCTION(Server, Reliable)
	void ServerEquipItem(TSubclassOf<UEquipmentDefinition> EquipmentDefinition);

	UFUNCTION(Server, Reliable)
	void ServerUnEquipItem(UEquipmentInstance* EquipmentInstance);

	
};
