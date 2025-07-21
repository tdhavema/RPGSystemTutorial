// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/ItemActor.h"

#include "Inventory/InventoryComponent.h"


AItemActor::AItemActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetReplicates(true);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetIsReplicated(true);
}

void AItemActor::SetParams(const FRPGInventoryEntry* Entry, int32 InNumItems)
{
	if (Entry)
	{
		ItemTag = Entry->ItemTag;
		EffectPackage = Entry->EffectPackage;
		NumItems = InNumItems;
	}
}

void AItemActor::SetMesh(UStaticMesh* InMesh)
{
	if (IsValid(Mesh))
	{
		Mesh->SetStaticMesh(InMesh);
	}
}

