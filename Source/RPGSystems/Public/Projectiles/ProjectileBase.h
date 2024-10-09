// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

struct FProjectileParams;
class UProjectileMovementComponent;

UCLASS()
class RPGSYSTEMS_API AProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:
	
	AProjectileBase();

	void SetProjectileParams(const FProjectileParams& Params);

private:

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;

	UPROPERTY()
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;


};
