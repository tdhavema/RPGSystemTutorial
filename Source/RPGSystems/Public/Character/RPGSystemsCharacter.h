// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "CharacterBase.h"
#include "GameplayAbilitySpecHandle.h"
#include "Interfaces/RPGAbilityInterface.h"
#include "RPGSystemsCharacter.generated.h"

class UGameplayAbility;
class URPGSystemsWidget;
class URPGAttributeSet;
class URPGAbilitySystemComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS(config=Game)
class ARPGSystemsCharacter : public ACharacterBase, public IAbilitySystemInterface, public IRPGAbilityInterface
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

public:
	
	ARPGSystemsCharacter();

	/* Implement RPG Ability Interface */
	virtual void SetDynamicProjectile_Implementation(const FGameplayTag& ProjectileTag) override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
			

	virtual void InitAbilityActorInfo() override;
	virtual void BindCallbacksToDependencies() override;
	virtual void InitClassDefaults() override;
	virtual void BroadcastInitialValues() override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;

private:

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<URPGAbilitySystemComponent> RPGAbilitySystemComp;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<URPGAttributeSet> RPGAttributes;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Dyanmic Projectile Ability")
	TSubclassOf<UGameplayAbility> DynamicProjectileAbility;

	FGameplayAbilitySpecHandle ActiveDynamicProjectileAbilityHandle;

	UFUNCTION(Server, Reliable)
	void ServerSetDynamicProjectile(const FGameplayTag& ProjectileTag);

public:
	
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

