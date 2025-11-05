// Fill out your copyright notice in the Description page of Project Settings.


#include "Crafting/CraftingNPC.h"


ACraftingNPC::ACraftingNPC()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	SetRootComponent(Mesh);
	Mesh->SetIsReplicated(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

}
