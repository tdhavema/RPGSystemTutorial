// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RPGCharacterMovement.generated.h"

/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API URPGCharacterMovement : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:

	virtual void Crouch(bool bClientSimulation = false) override;
	virtual void UnCrouch(bool bClientSimulation = false) override;
};
