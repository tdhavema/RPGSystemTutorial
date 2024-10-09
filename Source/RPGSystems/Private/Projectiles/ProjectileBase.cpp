// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/ProjectileBase.h"

#include "AbilitySystem/RPGAbilityTypes.h"
#include "GameFramework/ProjectileMovementComponent.h"


AProjectileBase::AProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>("ProjectileMesh");
	SetRootComponent(ProjectileMesh);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ProjectileMesh->SetCollisionObjectType(ECC_WorldDynamic);
	ProjectileMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ProjectileMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	ProjectileMesh->SetIsReplicated(true);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComponent");

}

void AProjectileBase::SetProjectileParams(const FProjectileParams& Params)
{
	if (IsValid(ProjectileMesh))
	{
		ProjectileMesh->SetStaticMesh(Params.ProjectileMesh);
	}

	if (IsValid(ProjectileMovementComponent))
	{
		ProjectileMovementComponent->InitialSpeed = Params.InitialSpeed;
		ProjectileMovementComponent->ProjectileGravityScale = Params.GravityScale;
		ProjectileMovementComponent->bShouldBounce = Params.bShouldBounce;
		ProjectileMovementComponent->Bounciness = Params.Bounciness;
	}
}
