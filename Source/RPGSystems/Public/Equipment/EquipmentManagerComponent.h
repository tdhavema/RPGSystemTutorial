// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipmentTypes.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "EquipmentManagerComponent.generated.h"

class URPGAbilitySystemComponent;
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

	UPROPERTY(BlueprintReadOnly)
	FEquipmentEffectPackage EffectPackage = FEquipmentEffectPackage();

	UPROPERTY(NotReplicated)
	FEquipmentGrantedHandles GrantedHandles = FEquipmentGrantedHandles();

	bool HasStats() const
	{
		return !EffectPackage.Prefixes.IsEmpty() || !EffectPackage.Suffixes.IsEmpty();
	}

	bool HasAbility() const
	{
		return EffectPackage.Ability.AbilityTag.IsValid();
	}

private:

	friend UEquipmentManagerComponent;
	friend struct FRPGEquipmentList;

	UPROPERTY()
	TSubclassOf<UEquipmentDefinition> EquipmentDefinition = nullptr;

	UPROPERTY()
	TObjectPtr<UEquipmentInstance> Instance = nullptr;
	
};

DECLARE_MULTICAST_DELEGATE_OneParam(FEquipmentEntrySignature, const FRPGEquipmentEntry& /* Equipment Entry */);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUnEquippedEntry, const FRPGEquipmentEntry& /* UnEquipped Entry */);

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

	URPGAbilitySystemComponent* GetAbilitySystemComponent();
	void AddEquipmentStats(FRPGEquipmentEntry* Entry);
	void RemoveEquipmentStats(FRPGEquipmentEntry* Entry);
	void AddEquipmentAbility(FRPGEquipmentEntry* Entry);
	void RemoveEquipmentAbility(FRPGEquipmentEntry* Entry);
	UEquipmentInstance* AddEntry(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition, const FEquipmentEffectPackage& EffectPackage);
	void RemoveEntry(UEquipmentInstance* EquipmentInstance);
	void BindAbilitySystemDelegates();
	void CheckAbilityLevels(UAbilitySystemComponent* ASC, FRPGEquipmentEntry* EquipmentEntry, bool bAsync);

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
	FOnUnEquippedEntry UnEquippedEntryDelegate;
	
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

	void EquipItem(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition, const FEquipmentEffectPackage& EffectPackage); 
	void UnEquipItem(UEquipmentInstance* EquipmentInstance);

protected:

	virtual void BeginPlay() override;

private:

	UFUNCTION(Server, Reliable)
	void ServerEquipItem(TSubclassOf<UEquipmentDefinition> EquipmentDefinition, const FEquipmentEffectPackage& EffectPackage); 
 
	UFUNCTION(Server, Reliable)
	void ServerUnEquipItem(UEquipmentInstance* EquipmentInstance);

	
};
