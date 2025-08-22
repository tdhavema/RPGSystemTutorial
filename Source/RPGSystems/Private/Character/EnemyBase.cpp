// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/EnemyBase.h"

#include "GameplayTagsManager.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/RPGAttributeSet.h"
#include "Data/CharacterClassInfo.h"
#include "Data/LootSpawnInfo.h"
#include "Inventory/InventoryComponent.h"
#include "Libraries/RPGAbilitySystemLibrary.h"
#include "Net/UnrealNetwork.h"

AEnemyBase::AEnemyBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;

	RPGAbilitySystemComp = CreateDefaultSubobject<URPGAbilitySystemComponent>("AbilitySystemComponent");
	RPGAbilitySystemComp->SetIsReplicated(true);
	RPGAbilitySystemComp->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	RPGAttributes = CreateDefaultSubobject<URPGAttributeSet>("AttributeSet");

	InventoryComp = CreateDefaultSubobject<UInventoryComponent>("InventoryComp");
}

void AEnemyBase::AddAttackingActor_Implementation(AActor* AttackingActor)
{
	if (!AttackingActors.Contains(AttackingActor))
	{
		AttackingActors.Emplace(AttackingActor);
	}
}

void AEnemyBase::Death_Implementation()
{
	if (!HasAuthority()) return;

	if (!AttackingActors.IsEmpty())
	{
		for (AActor* Attacker : AttackingActors)
		{
			if (IsValid(Attacker))
			{
				Execute_AddToExperience(Attacker, AwardedExperienceScale);
			}
		}
	}

	SpawnLoot();

	Destroy();
}

void AEnemyBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemyBase, bInitAttributes);
}

void AEnemyBase::OnRep_InitAttributes()
{
	BroadcastInitialValues();
}

UAbilitySystemComponent* AEnemyBase::GetAbilitySystemComponent() const
{
	return RPGAbilitySystemComp;
}

void AEnemyBase::SpawnLoot()
{
	check(InventoryComp);
	check(LootSpawnInfo);

	FGameplayTagContainer AllLootTags;
	AllLootTags.AppendTags(URPGAbilitySystemLibrary::GetAllChildrenTagsOfCategories(LootCategoryTags)); 
	AllLootTags.AppendTags(SpecificLootTags);

	bool bShouldSpawn = true;

	while (bShouldSpawn)
	{
		const FGameplayTag& RandomTag = URPGAbilitySystemLibrary::GetRandomTagFromContainer(AllLootTags);

		for (const auto& Pair : LootSpawnInfo->TaggedLootTables)
		{
			if (RandomTag.MatchesTag(Pair.Key))
			{
				if (const FPossibleLootParams* PossibleLoot = URPGAbilitySystemLibrary::GetDataTableRowByTag<FPossibleLootParams>(Pair.Value, RandomTag))
				{
					if (FMath::FRandRange(0.f, 1.f) < PossibleLoot->ProbabilityToSelect)
					{
						const int32 RandomNumItems = FMath::RandRange(PossibleLoot->MinNumItems, PossibleLoot->MaxNumItems);

						if (const FRPGInventoryEntry* Result = InventoryComp->InventoryList.AddItem(PossibleLoot->ResultingItemTag, RandomNumItems))
						{
							InventoryComp->SpawnItem(GetActorTransform(), Result, Result->Quantity);
							bShouldSpawn = false;
							break;
						}

						break;
					}

					break;
				}

				break;
			}
		}
	}
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	BindCallbacksToDependencies();
	InitAbilityActorInfo();
}

void AEnemyBase::InitAbilityActorInfo()
{
	if (IsValid(RPGAbilitySystemComp) && IsValid(RPGAttributes))
	{
		RPGAbilitySystemComp->InitAbilityActorInfo(this, this);

		if (HasAuthority())
		{
			InitClassDefaults();
			BroadcastInitialValues();
		}
	}
}

void AEnemyBase::InitClassDefaults()
{
	if (!CharacterTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Character Tag Selected In This Character %s"), *GetNameSafe(this));
	}
	else if (UCharacterClassInfo* ClassInfo = URPGAbilitySystemLibrary::GetCharacterClassDefaultInfo(this))
	{
		if (const FCharacterClassDefaultInfo* SelectedClass = ClassInfo->ClassDefaultInfoMap.Find(CharacterTag))
		{
			if (IsValid(RPGAbilitySystemComp))
			{
				RPGAbilitySystemComp->AddCharacterAbilities(SelectedClass->StartingAbilities);
				RPGAbilitySystemComp->AddCharacterPassiveAbilities(SelectedClass->StartingPassives);
				RPGAbilitySystemComp->InitializeDefaultAttributes(SelectedClass->DefaultAttributes);
			}

			AwardedExperienceScale = SelectedClass->AwardedExperienceScale;
		}
	}
}

void AEnemyBase::BindCallbacksToDependencies()
{
	if (IsValid(RPGAbilitySystemComp) && IsValid(RPGAttributes))
	{
		RPGAbilitySystemComp->GetGameplayAttributeValueChangeDelegate(RPGAttributes->GetHealthAttribute()).AddLambda(
			[this] (const FOnAttributeChangeData& Data)
			{
				OnHealthChanged(Data.NewValue, RPGAttributes->GetMaxHealth());
			});

		RPGAbilitySystemComp->GetGameplayAttributeValueChangeDelegate(RPGAttributes->GetShieldAttribute()).AddLambda(
			[this] (const FOnAttributeChangeData& Data)
			{
				OnShieldChanged(Data.NewValue, RPGAttributes->GetMaxShield());
			});

		RPGAbilitySystemComp->OnAttributesGiven.AddLambda(
			[this]
			{
				bInitAttributes = true;
			});
	}
}

void AEnemyBase::BroadcastInitialValues()
{
	if (IsValid(RPGAttributes))
	{
		OnHealthChanged(RPGAttributes->GetHealth(), RPGAttributes->GetMaxHealth());
		OnShieldChanged(RPGAttributes->GetShield(), RPGAttributes->GetMaxShield());
	}
}

