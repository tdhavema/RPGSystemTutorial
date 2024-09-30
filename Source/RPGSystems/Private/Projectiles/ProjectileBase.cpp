// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/ProjectileBase.h"


AProjectileBase::AProjectileBase()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

