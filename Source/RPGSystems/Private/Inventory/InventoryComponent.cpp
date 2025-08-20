// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"
#include "Data/EquipmentStatEffects.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Equipment/EquipmentDefinition.h"
#include "Inventory/ItemActor.h"
#include "Inventory/ItemTypesToTables.h"
#include "Libraries/RPGAbilitySystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "RPGSystems/RPGSystems.h"

namespace RPGGameplayTags::Static
{
	UE_DEFINE_GAMEPLAY_TAG_STATIC(Category_Equipment, "Item.Equipment");
}


FRPGInventoryEntry* FRPGInventoryList::AddItem(const FGameplayTag& ItemTag, int32 NumItems)
{
	if (ItemTag.MatchesTag(RPGGameplayTags::Static::Category_Equipment))
	{
		// Cannot stack these categories, do nothing.
	}
	else
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
				return &Entry;
			}
		}
	}
	
	const FMasterItemDefinition Item = OwnerComponent->GetItemDefinitionByTag(ItemTag);
	
	FRPGInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.ItemTag = ItemTag;
	NewEntry.ItemName = Item.ItemName;
	NewEntry.Quantity = NumItems;
	NewEntry.ItemID = GenerateID();

	if (NewEntry.ItemTag.MatchesTag(RPGGameplayTags::Static::Category_Equipment) && IsValid(WeakStats.Get()))
	{
		RollForStats(Item.EquipmentItemProps.EquipmentClass, &NewEntry);
	}

	if (OwnerComponent->GetOwner()->HasAuthority())
	{
		DirtyItemDelegate.Broadcast(NewEntry);
	}

	MarkItemDirty(NewEntry);

	return &NewEntry;
}

void FRPGInventoryList::RollForStats(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition, FRPGInventoryEntry* Entry)
{
	UEquipmentStatEffects* StatEffects = WeakStats.Get();
	const UEquipmentDefinition* EquipmentCDO = GetDefault<UEquipmentDefinition>(EquipmentDefinition);

	if (EquipmentCDO->bForceAbilityRoll || FMath::FRandRange(0.f, 1.f) < EquipmentCDO->ProbabilityToRollAbility)
	{
		bool bShouldRoll = true;
		while (bShouldRoll)
		{
			const FGameplayTag& RandomTag = URPGAbilitySystemLibrary::GetRandomTagFromContainer(EquipmentCDO->PossibleAbilityRolls);

			for (const auto& Pair : StatEffects->MasterStatMap)
			{
				if (RandomTag.MatchesTag(Pair.Key))
				{
					if (const FEquipmentAbilityGroup* PossibleAbility = URPGAbilitySystemLibrary::GetDataTableRowByTag<FEquipmentAbilityGroup>(Pair.Value, RandomTag))
					{
						if (FMath::FRandRange(0.f, 1.f) < PossibleAbility->ProbabilityToSelect)
						{
							Entry->EffectPackage.Ability = *PossibleAbility;
							bShouldRoll = false;
							break;
						}
					}
				}
			}
		}
		
	}

	const int32 NumStatsToRoll = FMath::RandRange(EquipmentCDO->MinPossibleStats, EquipmentCDO->MaxPossibleStats);
	int32 StatRollIndex = 0;
	FGameplayTagContainer PossibleStatContainer = EquipmentCDO->PossibleStatRolls;
	while (StatRollIndex < NumStatsToRoll)
	{
		
		const FGameplayTag& RandomTag = URPGAbilitySystemLibrary::GetRandomTagFromContainer(PossibleStatContainer);

		for (const auto& Pair : StatEffects->MasterStatMap)
		{
			if (RandomTag.MatchesTag(Pair.Key))
			{
				if (const FEquipmentStatEffectGroup* PossibleStat = URPGAbilitySystemLibrary::GetDataTableRowByTag<FEquipmentStatEffectGroup>(Pair.Value, RandomTag))
				{
					if (FMath::FRandRange(0.f, 1.f) < PossibleStat->ProbabilityToSelect)
					{
						FEquipmentStatEffectGroup NewStat = *PossibleStat;

						NewStat.CurrentValue = PossibleStat->bFractionalStat ? FMath::FRandRange(PossibleStat->MinStatLevel, PossibleStat->MaxStatLevel) :
						FMath::TruncToInt(FMath::FRandRange(PossibleStat->MinStatLevel, PossibleStat->MaxStatLevel));

						Entry->EffectPackage.StatEffects.Add(NewStat);
						++StatRollIndex;

						PossibleStatContainer.RemoveTag(RandomTag);
						FName RandomTagName = RandomTag.GetTagLeafName();
						FGameplayTagContainer TagsToRemove;
						
						for (const FGameplayTag& Tag : PossibleStatContainer)
						{
							if (Tag.GetTagLeafName() == RandomTagName)
							{
								TagsToRemove.AddTagFast(Tag);
							}
						}

						PossibleStatContainer.RemoveTags(TagsToRemove);
						
						break;
					}
				}
			}
		}
	}
}

void FRPGInventoryList::AddUnEquippedItem(const FGameplayTag& ItemTag,
	const FEquipmentEffectPackage& EffectPackage, int32 NumItems)
{
	const FMasterItemDefinition Item = OwnerComponent->GetItemDefinitionByTag(ItemTag);
	
	FRPGInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.ItemTag = ItemTag;
	NewEntry.ItemName = Item.ItemName;
	NewEntry.Quantity = NumItems;
	NewEntry.ItemID = GenerateID();
	NewEntry.EffectPackage = EffectPackage;

	DirtyItemDelegate.Broadcast(NewEntry);
	MarkItemDirty(NewEntry);
}

void FRPGInventoryList::RemoveItem(const FRPGInventoryEntry& InventoryEntry, int32 NumItems)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FRPGInventoryEntry& Entry = *EntryIt;

		if (Entry.ItemID == InventoryEntry.ItemID)
		{
			Entry.Quantity -= NumItems;

			if (Entry.Quantity > 0)
			{
				MarkItemDirty(Entry);

				if (OwnerComponent->GetOwner()->HasAuthority())
				{
					DirtyItemDelegate.Broadcast(Entry);
				}
			}
			else
			{
				InventoryItemRemovedDelegate.Broadcast(Entry.ItemID);
				EntryIt.RemoveCurrent();
				MarkArrayDirty();
			}
			break;
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

uint64 FRPGInventoryList::GenerateID()
{
	uint64 NewID = ++LastAssignedID;

	int32 SignatureIndex = 0;
	while (SignatureIndex < 12)
	{
		if (FMath::RandRange(0, 100) < 85)
		{
			NewID |= (uint64)1 << FMath::RandRange(0, 63);
		}
		++SignatureIndex;
	}

	return NewID;
}

void FRPGInventoryList::SetStats(UEquipmentStatEffects* InStats)
{
	WeakStats = InStats;
}

void FRPGInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (const int32 Index : RemovedIndices)
	{
		const FRPGInventoryEntry& Entry = Entries[Index];

		InventoryItemRemovedDelegate.Broadcast(Entry.ItemID);
	}
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

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		InventoryList.SetStats(StatEffects);
	}
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

void UInventoryComponent::UseItem(const FRPGInventoryEntry& Entry, int32 NumItems)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return;

	if (!Owner->HasAuthority())
	{
		ServerUseItem(Entry, NumItems);
		return;
	}
	
	if (InventoryList.HasEnough(Entry.ItemTag, NumItems))
	{
		const FMasterItemDefinition Item = GetItemDefinitionByTag(Entry.ItemTag);

		if (UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner))
		{
			if (IsValid(Item.ConsumableProps.ItemEffectClass))
			{
				const FGameplayEffectContextHandle ContextHandle = OwnerASC->MakeEffectContext();
			
				const FGameplayEffectSpecHandle SpecHandle = OwnerASC->MakeOutgoingSpec(Item.ConsumableProps.ItemEffectClass,
						Item.ConsumableProps.ItemEffectLevel, ContextHandle);
			
				OwnerASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

				InventoryList.RemoveItem(Entry);

				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta,
					FString::Printf(TEXT("Server Item Used: %s"), *Item.ItemTag.ToString()));
			}
			if (IsValid(Item.EquipmentItemProps.EquipmentClass))
			{
				EquipmentItemDelegate.Broadcast(Item.EquipmentItemProps.EquipmentClass, Entry.EffectPackage);
				InventoryList.RemoveItem(Entry);
			}
		}
	}
}

void UInventoryComponent::ServerUseItem_Implementation(const FRPGInventoryEntry& Entry, int32 NumItems)
{
	UseItem(Entry, NumItems);
}

FMasterItemDefinition UInventoryComponent::GetItemDefinitionByTag(const FGameplayTag& ItemTag) const
{
	checkf(InventoryDefinitions, TEXT("No Inventory Definitions Inside Component %s"), *GetNameSafe(this));

	for (const auto& Pair : InventoryDefinitions->TagsToTables)
	{
		if (ItemTag.MatchesTag(Pair.Key))
		{
			if (const FMasterItemDefinition* ValidItem = URPGAbilitySystemLibrary::GetDataTableRowByTag<FMasterItemDefinition>(Pair.Value, ItemTag))
			{
				return *ValidItem;
			}
		}
	}

	return FMasterItemDefinition();
}

TArray<FRPGInventoryEntry> UInventoryComponent::GetInventoryEntries()
{
	return InventoryList.Entries;
}

void UInventoryComponent::AddUnEquippedItemEntry(const FGameplayTag& ItemTag,
	const FEquipmentEffectPackage& EffectPackage)
{
	InventoryList.AddUnEquippedItem(ItemTag, EffectPackage);
}

TArray<FRPGInventoryEntry> UInventoryComponent::GetEntriesByString(const FString& String)
{
	TArray<FRPGInventoryEntry> MatchedEntries;

	for (auto EntryIt = InventoryList.Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		const FRPGInventoryEntry& Entry = *EntryIt;

		if (Entry.ItemName.ToString().Contains(String))
		{
			MatchedEntries.Add(Entry);
		}

		if (Entry.EffectPackage.Ability.AbilityName.ToString().Contains(String))
		{
			if (!MatchedEntries.Contains(Entry))
			{
				MatchedEntries.Add(Entry);
			}
		}

		for (const FEquipmentStatEffectGroup& StatEffect : Entry.EffectPackage.StatEffects)
		{
			if (StatEffect.StatEffectName.ToString().Contains(String))
			{
				if (!MatchedEntries.Contains(Entry))
				{
					MatchedEntries.Add(Entry);
				}
			}
		}
	}

	return MatchedEntries;
}

void UInventoryComponent::SpawnItem(const FTransform& SpawnTransform, const FRPGInventoryEntry* Entry, int32 NumItems)
{
	if (!IsValid(DefaultItemClass)) return;
	
	AItemActor* NewActor = GetWorld()->SpawnActorDeferred<AItemActor>(DefaultItemClass, SpawnTransform);

	NewActor->SetParams(Entry, NumItems);
	NewActor->ValidationBits |= SERVER_BITS;
	
	FMasterItemDefinition Item = GetItemDefinitionByTag(Entry->ItemTag);

	if (IsValid(Item.ItemMesh.Get()))
	{
		NewActor->SetMesh(Item.ItemMesh.Get());
		NewActor->FinishSpawning(SpawnTransform);
	}
	else
	{
		FStreamableManager& Manager = UAssetManager::GetStreamableManager();

		Manager.RequestAsyncLoad(Item.ItemMesh.ToSoftObjectPath(),
			[NewActor, Item, SpawnTransform]
			{
				NewActor->SetMesh(Item.ItemMesh.Get());
				NewActor->FinishSpawning(SpawnTransform);
			});
	}
}

bool UInventoryComponent::ServerUseItem_Validate(const FRPGInventoryEntry& Entry, int32 NumItems)
{
	return Entry.IsValid() && InventoryList.HasEnough(Entry.ItemTag, NumItems);
}

void UInventoryComponent::DropItem(const FRPGInventoryEntry& Entry, int32 NumItems)
{
	if (!GetOwner()->HasAuthority())
	{
		ServerDropItem(Entry, NumItems);
		return;
	}

	ItemDroppedDelegate.Broadcast(&Entry, NumItems);
	InventoryList.RemoveItem(Entry, NumItems);
}

void UInventoryComponent::ServerDropItem_Implementation(const FRPGInventoryEntry& Entry, int32 NumItems)
{
	DropItem(Entry, NumItems);
}

void UInventoryComponent::PickupItem(AItemActor* Item)
{
	if (!IsValid(Item)) return;

	if (!GetOwner()->HasAuthority())
	{
		ServerPickupItem(Item);
		return;
	}

	InventoryList.AddUnEquippedItem(Item->ItemTag, Item->EffectPackage, Item->NumItems);

	Item->Destroy();
}

void UInventoryComponent::ServerPickupItem_Implementation(AItemActor* Item)
{
	PickupItem(Item);
}

bool UInventoryComponent::ServerPickupItem_Validate(AItemActor* Item)
{
	return Item->ValidationBits & SERVER_BITS;
}
