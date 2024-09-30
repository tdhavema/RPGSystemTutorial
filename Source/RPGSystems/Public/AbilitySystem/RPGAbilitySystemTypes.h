// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGAbilitySystemTypes.generated.h"

class UGameplayAbility;
class AProjectileBase;

USTRUCT()
struct FProjectileParams
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AProjectileBase> ProjectileClass;

	UPROPERTY(EditDefaultsOnly)
	float InitialSpeed = 500.f;

	UPROPERTY(EditDefaultsOnly)
	float GravityScale = 1.f;

	UPROPERTY(EditDefaultsOnly)
	bool bShouldBounce = false;

	UPROPERTY(EditDefaultsOnly)
	float Bounciness = 0.6f;
	
};
