// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Inventory/ItemTypesToTables.h"
#include "Libraries/RPGAbilitySystemLibrary.h"
#include "Net/UnrealNetwork.h"


void FRPGInventoryList::AddItem(const FGameplayTag& ItemTag, int32 NumItems)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FRPGInventoryEntry& Entry = *EntryIt;

		if (Entry.ItemTag.MatchesTagExact(ItemTag))
		{
			Entry.Quantity += NumItems;
			MarkItemDirty(Entry);

			if (OwnerComponent->GetOwner()->HasAuthority())
			{
				DirtyItemDelegate.Broadcast(Entry);
			}
			return;
		}
	}

	FRPGInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.ItemTag = ItemTag;
	NewEntry.Quantity = NumItems;

	if (OwnerComponent->GetOwner()->HasAuthority())
	{
		DirtyItemDelegate.Broadcast(NewEntry);
	}

	MarkItemDirty(NewEntry);
}

void FRPGInventoryList::RemoveItem(const FGameplayTag& ItemTag, int32 NumItems)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FRPGInventoryEntry& Entry = *EntryIt;

		if (Entry.ItemTag.MatchesTagExact(ItemTag))
		{
			Entry.Quantity -= NumItems;

			MarkItemDirty(Entry);

			if (OwnerComponent->GetOwner()->HasAuthority())
			{
				DirtyItemDelegate.Broadcast(Entry);
			}
		}
	}
}

bool FRPGInventoryList::HasEnough(const FGameplayTag& ItemTag, int32 NumItems)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FRPGInventoryEntry& Entry = *EntryIt;

		if (Entry.ItemTag.MatchesTagExact(ItemTag))
		{
			if (Entry.Quantity >= NumItems)
			{
				return true;
			}
		}
	}

	return false;
}

void FRPGInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	// If you can figure out what to do with this go for it. I don't know what it is reliably good for.
}

void FRPGInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const int32 Index : AddedIndices)
	{
		FRPGInventoryEntry& Entry = Entries[Index];

		DirtyItemDelegate.Broadcast(Entry);
	}
}

void FRPGInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (const int32 Index : ChangedIndices)
	{
		FRPGInventoryEntry& Entry = Entries[Index];

		DirtyItemDelegate.Broadcast(Entry);
	}
}

UInventoryComponent::UInventoryComponent() :
	InventoryList(this)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, InventoryList);
}

void UInventoryComponent::AddItem(const FGameplayTag& ItemTag, int32 NumItems)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return;

	if (!Owner->HasAuthority())
	{
		ServerAddItem(ItemTag, NumItems);
		return;
	}

	InventoryList.AddItem(ItemTag, NumItems);
}

void UInventoryComponent::ServerAddItem_Implementation(const FGameplayTag& ItemTag, int32 NumItems)
{
	AddItem(ItemTag, NumItems);
}

void UInventoryComponent::UseItem(const FGameplayTag& ItemTag, int32 NumItems)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return;

	if (!Owner->HasAuthority())
	{
		ServerUseItem(ItemTag, NumItems);
		return;
	}
	
	if (InventoryList.HasEnough(ItemTag, NumItems))
	{
		const FMasterItemDefinition Item = GetItemDefinitionByTag(ItemTag);

		if (UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner))
		{
			if (IsValid(Item.ConsumableProps.ItemEffectClass))
			{
				const FGameplayEffectContextHandle ContextHandle = OwnerASC->MakeEffectContext();
			
				const FGameplayEffectSpecHandle SpecHandle = OwnerASC->MakeOutgoingSpec(Item.ConsumableProps.ItemEffectClass,
						Item.ConsumableProps.ItemEffectLevel, ContextHandle);
			
				OwnerASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

				InventoryList.RemoveItem(ItemTag);

				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta,
					FString::Printf(TEXT("Server Item Used: %s"), *Item.ItemTag.ToString()));
			}
		}
	}
}

void UInventoryComponent::ServerUseItem_Implementation(const FGameplayTag& ItemTag, int32 NumItems)
{
	if (InventoryList.HasEnough(ItemTag, NumItems))
	{
		UseItem(ItemTag, NumItems);
	}
}

FMasterItemDefinition UInventoryComponent::GetItemDefinitionByTag(const FGameplayTag& ItemTag) const
{
	checkf(InventoryDefinitions, TEXT("No Inventory Definitions Inside Component %s"), *GetNameSafe(this));

	for (const auto& Pair : InventoryDefinitions->TagsToTables)
	{
		if (ItemTag.MatchesTag(Pair.Key))
		{
			return *URPGAbilitySystemLibrary::GetDataTableRowByTag<FMasterItemDefinition>(Pair.Value, ItemTag);
		}
	}

	return FMasterItemDefinition();
}

TArray<FRPGInventoryEntry> UInventoryComponent::GetInventoryEntries()
{
	return InventoryList.Entries;
}
