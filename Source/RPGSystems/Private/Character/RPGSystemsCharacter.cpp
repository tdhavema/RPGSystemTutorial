// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/RPGSystemsCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Game/PlayerState/RPGPlayerState.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "Data/CharacterClassInfo.h"
#include "Libraries/RPGAbilitySystemLibrary.h"
#include "AbilitySystem/Attributes/RPGAttributeSet.h"
#include "Character/Animation/RPGAnimInstance.h"

ARPGSystemsCharacter::ARPGSystemsCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	FollowCamera->bUsePawnControlRotation = false;

	DynamicProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>("ProjectileSpawnPoint");
	DynamicProjectileSpawnPoint->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

}

USceneComponent* ARPGSystemsCharacter::GetDynamicSpawnPoint_Implementation()
{
	return DynamicProjectileSpawnPoint;
}

void ARPGSystemsCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (HasAuthority())
	{
		InitAbilityActorInfo();
	}
}

void ARPGSystemsCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	InitAbilityActorInfo();
}

void ARPGSystemsCharacter::InitAbilityActorInfo()
{
	if (ARPGPlayerState* RPGPlayerState = GetPlayerState<ARPGPlayerState>())
	{
		RPGAbilitySystemComp = RPGPlayerState->GetRPGAbilitySystemComponent();
		RPGAttributes = RPGPlayerState->GetRPGAttributes();

		if (IsValid(RPGAbilitySystemComp))
		{
			RPGAbilitySystemComp->InitAbilityActorInfo(RPGPlayerState, this);
			BindCallbacksToDependencies();

			if (HasAuthority())
			{
				InitClassDefaults();
			}

			if (URPGAnimInstance* RPGAnimInstance = Cast<URPGAnimInstance>(GetMesh()->GetAnimInstance()))
			{
				RPGAnimInstance->InitializeWithAbilitySystem(RPGAbilitySystemComp);
			}
		}
	}
}

void ARPGSystemsCharacter::InitClassDefaults()
{
	if (!CharacterTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Character Tag Selected In This Character %s"), *GetNameSafe(this));
	}
	else if (UCharacterClassInfo* ClassInfo = URPGAbilitySystemLibrary::GetCharacterClassDefaultInfo(this))
	{
		if (const FCharacterClassDefaultInfo* SelectedClassInfo = ClassInfo->ClassDefaultInfoMap.Find(CharacterTag))
		{
			if (IsValid(RPGAbilitySystemComp))
			{
				RPGAbilitySystemComp->AddCharacterAbilities(SelectedClassInfo->StartingAbilities);
				RPGAbilitySystemComp->AddCharacterPassiveAbilities(SelectedClassInfo->StartingPassives);
				RPGAbilitySystemComp->InitializeDefaultAttributes(SelectedClassInfo->DefaultAttributes);
			}
		}
	}
}

void ARPGSystemsCharacter::BindCallbacksToDependencies()
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
	}
}

void ARPGSystemsCharacter::BroadcastInitialValues()
{
	if (IsValid(RPGAttributes))
	{
		OnHealthChanged(RPGAttributes->GetHealth(), RPGAttributes->GetMaxHealth());
		OnShieldChanged(RPGAttributes->GetShield(), RPGAttributes->GetMaxShield());
	}
}

UAbilitySystemComponent* ARPGSystemsCharacter::GetAbilitySystemComponent() const
{
	return RPGAbilitySystemComp;
}

void ARPGSystemsCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority()) return;
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ARPGSystemsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARPGSystemsCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARPGSystemsCharacter::Look);
	}
}

void ARPGSystemsCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ARPGSystemsCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}