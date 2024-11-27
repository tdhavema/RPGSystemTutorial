// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EquipmentManagerComponent.h"

#include "Equipment/EquipmentDefinition.h"
#include "Equipment/EquipmentInstance.h"
#include "Net/UnrealNetwork.h"

UEquipmentInstance* FRPGEquipmentList::AddEntry(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition)
{
	check(EquipmentDefinition);
	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());

	const UEquipmentDefinition* EquipmentCDO = GetDefault<UEquipmentDefinition>(EquipmentDefinition);
	TSubclassOf<UEquipmentInstance> InstanceType = EquipmentCDO->InstanceType;

	if (!IsValid(InstanceType))
	{
		InstanceType = UEquipmentInstance::StaticClass();
	}

	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FRPGEquipmentEntry& Entry = *EntryIt;

		if (Entry.SlotTag.MatchesTagExact(EquipmentCDO->SlotTag))
		{
			RemoveEntry(Entry.Instance);
			break;
		}
	}

	FRPGEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.EntryTag = EquipmentCDO->ItemTag;
	NewEntry.RarityTag = EquipmentCDO->RarityTag;
	NewEntry.SlotTag = EquipmentCDO->SlotTag;
	NewEntry.EquipmentDefinition = EquipmentDefinition;
	NewEntry.Instance = NewObject<UEquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);

	MarkItemDirty(NewEntry);
	EquipmentEntryDelegate.Broadcast(NewEntry);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
		FString::Printf(TEXT("Equipped Item: %s"), *NewEntry.EntryTag.ToString()));

	return NewEntry.Instance;
	
}

void FRPGEquipmentList::RemoveEntry(UEquipmentInstance* EquipmentInstance)
{
	check(OwnerComponent);

	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FRPGEquipmentEntry& Entry = *EntryIt;

		if (Entry.Instance == EquipmentInstance)
		{
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

void FRPGEquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (const int32 Index : RemovedIndices)
	{
		FRPGEquipmentEntry& Entry = Entries[Index];

		EquipmentEntryDelegate.Broadcast(Entry);

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
		FString::Printf(TEXT("UnEquipped Item: %s"), *Entry.EntryTag.ToString()));
	}
}

void FRPGEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const int32 Index : AddedIndices)
	{
		FRPGEquipmentEntry& Entry = Entries[Index];

		EquipmentEntryDelegate.Broadcast(Entry);

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
		FString::Printf(TEXT("Equipped Item: %s"), *Entry.EntryTag.ToString()));
	}
}

void FRPGEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (const int32 Index : ChangedIndices)
	{
		FRPGEquipmentEntry& Entry = Entries[Index];

		EquipmentEntryDelegate.Broadcast(Entry);
	}
}

UEquipmentManagerComponent::UEquipmentManagerComponent() :
	EquipmentList(this)
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

}

void UEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UEquipmentManagerComponent, EquipmentList);
}

void UEquipmentManagerComponent::EquipItem(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition)
{
	if (!GetOwner()->HasAuthority())
	{
		ServerEquipItem(EquipmentDefinition);
		return;
	}

	EquipmentList.AddEntry(EquipmentDefinition);
	
}

void UEquipmentManagerComponent::UnEquipItem(UEquipmentInstance* EquipmentInstance)
{
	if (!GetOwner()->HasAuthority())
	{
		ServerUnEquipItem(EquipmentInstance);
		return;
	}

	EquipmentList.RemoveEntry(EquipmentInstance);
}

void UEquipmentManagerComponent::ServerEquipItem_Implementation(
	TSubclassOf<UEquipmentDefinition> EquipmentDefinition)
{
	EquipItem(EquipmentDefinition);
}

void UEquipmentManagerComponent::ServerUnEquipItem_Implementation(UEquipmentInstance* EquipmentInstance)
{
	UnEquipItem(EquipmentInstance);
}

