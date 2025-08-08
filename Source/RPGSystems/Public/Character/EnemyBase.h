// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ScalableFloat.h"
#include "Character/CharacterBase.h"
#include "Interfaces/RPGAbilitySystemInterface.h"
#include "EnemyBase.generated.h"

class URPGAttributeSet;
class URPGAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API AEnemyBase : public ACharacterBase, public IAbilitySystemInterface, public IRPGAbilitySystemInterface
{
	GENERATED_BODY()

public:

	AEnemyBase(const FObjectInitializer& ObjectInitializer);

	/* Implement RPG Ability System Interface */
	virtual void AddAttackingActor_Implementation(AActor* AttackingActor) override;
	virtual void Death_Implementation() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:

	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitClassDefaults() override;
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override; 

private:
	
	UPROPERTY(ReplicatedUsing=OnRep_InitAttributes)
	bool bInitAttributes = false;
	
	UFUNCTION()
	void OnRep_InitAttributes();
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<URPGAbilitySystemComponent> RPGAbilitySystemComp;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<URPGAttributeSet> RPGAttributes;

	FScalableFloat AwardedExperienceScale;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> AttackingActors;

};
