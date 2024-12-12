// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EquipmentActor.h"


AEquipmentActor::AEquipmentActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);

	RootScene = CreateDefaultSubobject<USceneComponent>("RootScene");
	SetRootComponent(RootScene);

	EquipmentMesh = CreateDefaultSubobject<UStaticMeshComponent>("EquipmentMesh");
	EquipmentMesh->SetupAttachment(GetRootComponent());
	EquipmentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

