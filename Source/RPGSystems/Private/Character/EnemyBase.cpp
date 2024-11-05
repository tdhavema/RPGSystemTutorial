// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/EnemyBase.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/RPGAttributeSet.h"
#include "Data/CharacterClassInfo.h"
#include "Libraries/RPGAbilitySystemLibrary.h"
#include "Net/UnrealNetwork.h"

AEnemyBase::AEnemyBase()
{
	bReplicates = true;

	RPGAbilitySystemComp = CreateDefaultSubobject<URPGAbilitySystemComponent>("AbilitySystemComponent");
	RPGAbilitySystemComp->SetIsReplicated(true);
	RPGAbilitySystemComp->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	RPGAttributes = CreateDefaultSubobject<URPGAttributeSet>("AttributeSet");
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
