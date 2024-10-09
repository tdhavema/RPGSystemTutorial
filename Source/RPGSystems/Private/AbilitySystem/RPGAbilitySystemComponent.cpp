// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/RPGAbilitySystemComponent.h"

#include "AbilitySystem/Abilities/ProjectileAbility.h"
#include "AbilitySystem/Abilities/RPGGameplayAbility.h"

void URPGAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& AbilitiesToGrant)
{
	for (const TSubclassOf<UGameplayAbility>& Ability : AbilitiesToGrant)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, 1.f);

		if (const URPGGameplayAbility* RPGAbility = Cast<URPGGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(RPGAbility->InputTag);
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
		if (Spec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			if (!Spec.IsActive())
			{
				TryActivateAbility(Spec.Handle);
			}
			else
			{
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle,
					Spec.ActivationInfo.GetActivationPredictionKey());
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
		if (Spec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle,
				Spec.ActivationInfo.GetActivationPredictionKey());
		}
	}
}

void URPGAbilitySystemComponent::SetDynamicProjectile(const FGameplayTag& ProjectileTag)
{
	if (!ProjectileTag.IsValid()) return;
	
	if (!GetAvatarActor()->HasAuthority())
	{
		ServerSetDynamicProjectile(ProjectileTag);
		return;
	}

	if (ActiveProjectileAbility.IsValid())
	{
		ClearAbility(ActiveProjectileAbility);
	}

	if (IsValid(DynamicProjectileAbility))
	{
		FGameplayAbilitySpec Spec = FGameplayAbilitySpec(DynamicProjectileAbility, 1);
		if (UProjectileAbility* ProjectileAbility = Cast<UProjectileAbility>(Spec.Ability))
		{
			ProjectileAbility->ProjectileToSpawnTag = ProjectileTag;
			Spec.DynamicAbilityTags.AddTag(ProjectileAbility->InputTag);

			ActiveProjectileAbility = GiveAbility(Spec);
		}
	}
	
}

void URPGAbilitySystemComponent::ServerSetDynamicProjectile_Implementation(const FGameplayTag& ProjectileTag)
{
	SetDynamicProjectile(ProjectileTag);
}
