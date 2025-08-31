// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/RPGAbilitySystemComponent.h"

#include "NativeGameplayTags.h"
#include "AbilitySystem/Abilities/ProjectileAbility.h"
#include "AbilitySystem/Abilities/RPGGameplayAbility.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Equipment/EquipmentManagerComponent.h"

namespace RPGGameplayTags::Static
{
	UE_DEFINE_GAMEPLAY_TAG_STATIC(StatEffect_Category_Attribute, "StatEffect.Attribute");
	UE_DEFINE_GAMEPLAY_TAG_STATIC(StatEffect_Category_Ability, "StatEffect.Ability");
}

void URPGAbilitySystemComponent::OnRep_ActivateAbilities()
{
	ABILITYLIST_SCOPE_LOCK();

	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.IsActive()) continue;

		TArray<UGameplayAbility*> Instances = Spec.GetAbilityInstances();

		if (URPGGameplayAbility* RPGAbility = Cast<URPGGameplayAbility>(Instances.Last()))
		{
			if (RPGAbility->bIsClientPassive)
			{
				TryActivateAbility(Spec.Handle);
			}
		}
	}
}

void URPGAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& AbilitiesToGrant)
{
	for (const TSubclassOf<UGameplayAbility>& Ability : AbilitiesToGrant)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, 1.f);

		if (const URPGGameplayAbility* RPGAbility = Cast<URPGGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(RPGAbility->InputTag);
			GiveAbility(AbilitySpec);
		}
	}
}

void URPGAbilitySystemComponent::AddCharacterPassiveAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& PassivesToGrant)
{
	for (const TSubclassOf<UGameplayAbility>& Ability : PassivesToGrant)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, 1.f);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void URPGAbilitySystemComponent::InitializeDefaultAttributes(const TSubclassOf<UGameplayEffect>& AttributeEffect)
{
	checkf(AttributeEffect, TEXT("No valid default attributes for this character %s"), *GetAvatarActor()->GetName());
	
	const FGameplayEffectContextHandle ContextHandle = MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(AttributeEffect, 1.f, ContextHandle);
	ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	OnAttributesGiven.Broadcast();
}

void URPGAbilitySystemComponent::AbilityInputPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	
	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			if (!Spec.IsActive())
			{
				TryActivateAbility(Spec.Handle);
			}
			else
			{
				TArray<UGameplayAbility*> Instances = Spec.GetAbilityInstances();
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle,
					Instances.Last()->GetCurrentActivationInfo().GetActivationPredictionKey());
			}
		}
	}
}

void URPGAbilitySystemComponent::AbilityInputReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			TArray<UGameplayAbility*> Instances = Spec.GetAbilityInstances();
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle,
				Instances.Last()->GetCurrentActivationInfo().GetActivationPredictionKey());
		}
	}
}

void URPGAbilitySystemComponent::SetDynamicProjectile(const FGameplayTag& ProjectileTag, int32 AbilityLevel)
{
	if (!ProjectileTag.IsValid()) return;
	
	if (!GetAvatarActor()->HasAuthority())
	{
		ServerSetDynamicProjectile(ProjectileTag, AbilityLevel);
		return;
	}

	if (ActiveProjectileAbility.IsValid())
	{
		ClearAbility(ActiveProjectileAbility);
	}

	if (IsValid(DynamicProjectileAbility))
	{
		FGameplayAbilitySpec Spec = FGameplayAbilitySpec(DynamicProjectileAbility, AbilityLevel);
		if (UProjectileAbility* ProjectileAbility = Cast<UProjectileAbility>(Spec.Ability))
		{
			ProjectileAbility->ProjectileToSpawnTag = ProjectileTag;
			Spec.GetDynamicSpecSourceTags().AddTag(ProjectileAbility->InputTag);

			ActiveProjectileAbility = GiveAbility(Spec);
		}
	}
	
}

void URPGAbilitySystemComponent::AddEquipmentEffects(FRPGEquipmentEntry* EquipmentEntry)
{
	FStreamableManager& Manager = UAssetManager::GetStreamableManager();
	TWeakObjectPtr<URPGAbilitySystemComponent> WeakThis(this);
	
	const FGameplayEffectContextHandle ContextHandle = MakeEffectContext();

	const FEquipmentStatEffectGroup& BaseDamageStat = EquipmentEntry->EffectPackage.BaseDamage;

	if (BaseDamageStat.StatEffectTag.IsValid())
	{
		if (IsValid(BaseDamageStat.EffectClass.Get()))
		{
			const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(BaseDamageStat.EffectClass.Get(), BaseDamageStat.CurrentValue, ContextHandle);
			const FActiveGameplayEffectHandle ActiveHandle = ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

			EquipmentEntry->GrantedHandles.AddEffectHandle(ActiveHandle);
		}
		else
		{
			Manager.RequestAsyncLoad(BaseDamageStat.EffectClass.ToSoftObjectPath(),
				[WeakThis, BaseDamageStat, ContextHandle, EquipmentEntry]
				{
					const FGameplayEffectSpecHandle SpecHandle = WeakThis->MakeOutgoingSpec(BaseDamageStat.EffectClass.Get(), BaseDamageStat.CurrentValue, ContextHandle);
					const FActiveGameplayEffectHandle ActiveHandle = WeakThis->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

					EquipmentEntry->GrantedHandles.AddEffectHandle(ActiveHandle);
				});
		}
	}

	const FEquipmentStatEffectGroup& ImplicitStat = EquipmentEntry->EffectPackage.Implicit;

	if (ImplicitStat.StatEffectTag.IsValid())
	{
		if (ImplicitStat.StatEffectTag.MatchesTag(RPGGameplayTags::Static::StatEffect_Category_Attribute))
		{
			if (IsValid(ImplicitStat.EffectClass.Get()))
			{
				const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(ImplicitStat.EffectClass.Get(), ImplicitStat.CurrentValue, ContextHandle);
				const FActiveGameplayEffectHandle ActiveHandle = ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

				EquipmentEntry->GrantedHandles.AddEffectHandle(ActiveHandle);
			}
			else
			{
				Manager.RequestAsyncLoad(ImplicitStat.EffectClass.ToSoftObjectPath(),
					[WeakThis, ImplicitStat, ContextHandle, EquipmentEntry]
					{
						const FGameplayEffectSpecHandle SpecHandle = WeakThis->MakeOutgoingSpec(ImplicitStat.EffectClass.Get(), ImplicitStat.CurrentValue, ContextHandle);
						const FActiveGameplayEffectHandle ActiveHandle = WeakThis->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

						EquipmentEntry->GrantedHandles.AddEffectHandle(ActiveHandle);
					});
			}
		}
		else if (ImplicitStat.StatEffectTag.MatchesTag(RPGGameplayTags::Static::StatEffect_Category_Ability))
		{
			for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
			{
				if (Spec.GetDynamicSpecSourceTags().HasTagExact(ImplicitStat.ContextTag))
				{
					Spec.Level = FMath::Clamp(Spec.Level + ImplicitStat.CurrentValue, 1.f, Spec.Level + ImplicitStat.CurrentValue);
				}
			}
		}
	}
	
	for (const FEquipmentStatEffectGroup& StatEffect : EquipmentEntry->EffectPackage.Prefixes)
	{
		if (StatEffect.StatEffectTag.MatchesTag(RPGGameplayTags::Static::StatEffect_Category_Attribute))
		{
			if (IsValid(StatEffect.EffectClass.Get()))
			{
				const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(StatEffect.EffectClass.Get(), StatEffect.CurrentValue, ContextHandle);
				const FActiveGameplayEffectHandle ActiveHandle = ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

				EquipmentEntry->GrantedHandles.AddEffectHandle(ActiveHandle);
			}
			else
			{
				Manager.RequestAsyncLoad(StatEffect.EffectClass.ToSoftObjectPath(),
					[WeakThis, StatEffect, ContextHandle, EquipmentEntry]
					{
						const FGameplayEffectSpecHandle SpecHandle = WeakThis->MakeOutgoingSpec(StatEffect.EffectClass.Get(), StatEffect.CurrentValue, ContextHandle);
						const FActiveGameplayEffectHandle ActiveHandle = WeakThis->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

						EquipmentEntry->GrantedHandles.AddEffectHandle(ActiveHandle);
					});
			}
		}
		else if (StatEffect.StatEffectTag.MatchesTag(RPGGameplayTags::Static::StatEffect_Category_Ability))
		{
			if (!StatEffect.ContextTag.IsValid()) continue;
			
			for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
			{
				if (Spec.GetDynamicSpecSourceTags().HasTagExact(StatEffect.ContextTag))
				{
					Spec.Level = FMath::Clamp(Spec.Level + StatEffect.CurrentValue, 1.f, Spec.Level + StatEffect.CurrentValue);
				}
			}
		}
	}

	for (const FEquipmentStatEffectGroup& StatEffect : EquipmentEntry->EffectPackage.Suffixes)
	{
		if (StatEffect.StatEffectTag.MatchesTag(RPGGameplayTags::Static::StatEffect_Category_Attribute))
		{
			if (IsValid(StatEffect.EffectClass.Get()))
			{
				const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(StatEffect.EffectClass.Get(), StatEffect.CurrentValue, ContextHandle);
				const FActiveGameplayEffectHandle ActiveHandle = ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

				EquipmentEntry->GrantedHandles.AddEffectHandle(ActiveHandle);
			}
			else
			{
				Manager.RequestAsyncLoad(StatEffect.EffectClass.ToSoftObjectPath(),
					[WeakThis, StatEffect, ContextHandle, EquipmentEntry]
					{
						const FGameplayEffectSpecHandle SpecHandle = WeakThis->MakeOutgoingSpec(StatEffect.EffectClass.Get(), StatEffect.CurrentValue, ContextHandle);
						const FActiveGameplayEffectHandle ActiveHandle = WeakThis->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

						EquipmentEntry->GrantedHandles.AddEffectHandle(ActiveHandle);
					});
			}
		}
		else if (StatEffect.StatEffectTag.MatchesTag(RPGGameplayTags::Static::StatEffect_Category_Ability))
		{
			if (!StatEffect.ContextTag.IsValid()) continue;
			
			for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
			{
				if (Spec.GetDynamicSpecSourceTags().HasTagExact(StatEffect.ContextTag))
				{
					Spec.Level = FMath::Clamp(Spec.Level + StatEffect.CurrentValue, 1.f, Spec.Level + StatEffect.CurrentValue);
				}
			}
		}
	}
}

void URPGAbilitySystemComponent::RemoveEquipmentEffects(FRPGEquipmentEntry* EquipmentEntry)
{
	for (auto HandleIt = EquipmentEntry->GrantedHandles.ActiveEffects.CreateIterator(); HandleIt; ++HandleIt)
	{
		RemoveActiveGameplayEffect(*HandleIt);
		HandleIt.RemoveCurrent();
	}

	const FEquipmentStatEffectGroup& ImplicitStat = EquipmentEntry->EffectPackage.Implicit;
	
	if (ImplicitStat.StatEffectTag.IsValid())
	{
		if (ImplicitStat.StatEffectTag.MatchesTag(RPGGameplayTags::Static::StatEffect_Category_Ability))
		{
			for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
			{
				if (Spec.GetDynamicSpecSourceTags().HasTagExact(ImplicitStat.ContextTag))
				{
					Spec.Level = FMath::Clamp(Spec.Level - ImplicitStat.CurrentValue, 1.f, Spec.Level - ImplicitStat.CurrentValue);
				}
			}
		}
	}

	for (const FEquipmentStatEffectGroup& StatEffect : EquipmentEntry->EffectPackage.Prefixes)
	{
		if (StatEffect.StatEffectTag.MatchesTag(RPGGameplayTags::Static::StatEffect_Category_Ability))
		{
			if (!StatEffect.ContextTag.IsValid()) continue;
			
			for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
			{
				if (Spec.GetDynamicSpecSourceTags().HasTagExact(StatEffect.ContextTag))
				{
					Spec.Level = FMath::Clamp(Spec.Level - StatEffect.CurrentValue, 1.f, Spec.Level - StatEffect.CurrentValue);
				}
			}
		}
	}

	for (const FEquipmentStatEffectGroup& StatEffect : EquipmentEntry->EffectPackage.Suffixes)
	{
		if (StatEffect.StatEffectTag.MatchesTag(RPGGameplayTags::Static::StatEffect_Category_Ability))
		{
			if (!StatEffect.ContextTag.IsValid()) continue;
			
			for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
			{
				if (Spec.GetDynamicSpecSourceTags().HasTagExact(StatEffect.ContextTag))
				{
					Spec.Level = FMath::Clamp(Spec.Level - StatEffect.CurrentValue, 1.f, Spec.Level - StatEffect.CurrentValue);
				}
			}
		}
	}
}

void URPGAbilitySystemComponent::AddEquipmentAbility(FRPGEquipmentEntry* EquipmentEntry)
{
	FStreamableManager& Manager = UAssetManager::GetStreamableManager();
	TWeakObjectPtr<URPGAbilitySystemComponent> WeakThis(this);

	if (IsValid(EquipmentEntry->EffectPackage.Ability.AbilityClass.Get()))
	{
		EquipmentEntry->GrantedHandles.GrantedAbility = GrantEquipmentAbility(EquipmentEntry);
		OnEquipmentAbilityGiven.Broadcast(EquipmentEntry, false);
	}
	else
	{
		Manager.RequestAsyncLoad(EquipmentEntry->EffectPackage.Ability.AbilityClass.ToSoftObjectPath(),
			[WeakThis, EquipmentEntry]
			{
				EquipmentEntry->GrantedHandles.GrantedAbility = WeakThis->GrantEquipmentAbility(EquipmentEntry);
				WeakThis->OnEquipmentAbilityGiven.Broadcast(EquipmentEntry, true);
			});
	}
}

void URPGAbilitySystemComponent::RemoveEquipmentAbility(const FRPGEquipmentEntry* EquipmentEntry)
{
	ClearAbility(EquipmentEntry->GrantedHandles.GrantedAbility);
}

FGameplayAbilitySpecHandle URPGAbilitySystemComponent::GrantEquipmentAbility(const FRPGEquipmentEntry* EquipmentEntry)
{
	FEquipmentAbilityGroup Ability = EquipmentEntry->EffectPackage.Ability;
	
	FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability.AbilityClass.Get(), Ability.AbilityLevel);

	AbilitySpec.GetDynamicSpecSourceTags().AddTag(Ability.AbilityTag);
	AbilitySpec.GetDynamicSpecSourceTags().AppendTags(Ability.AbilityContextTags);
	
	if (URPGGameplayAbility* RPGAbility = Cast<URPGGameplayAbility>(AbilitySpec.Ability))
	{
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(RPGAbility->InputTag);
	}

	if (URPGDamageAbility* DamageAbility = Cast<URPGDamageAbility>(AbilitySpec.Ability))
	{
		DamageAbility->MinDamageCoefficient = Ability.MinDamageCoefficient;
		DamageAbility->MaxDamageCoefficient = Ability.MaxDamageCoefficient;
	}
	if (UProjectileAbility* ProjectileAbility = Cast<UProjectileAbility>(AbilitySpec.Ability))
	{
		ProjectileAbility->ProjectileToSpawnTag = Ability.ContextTag;
	}

	return GiveAbility(AbilitySpec);
}

void URPGAbilitySystemComponent::ServerSetDynamicProjectile_Implementation(const FGameplayTag& ProjectileTag, int32 AbilityLevel)
{
	SetDynamicProjectile(ProjectileTag, AbilityLevel);
}
