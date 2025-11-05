// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractInterface.h"
#include "CraftingNPC.generated.h"

class UBoxComponent;

UCLASS()
class RPGSYSTEMS_API ACraftingNPC : public AActor, public IInteractInterface
{
	GENERATED_BODY()

public:

	ACraftingNPC();

private:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Custom Values|Name", meta = (AllowPrivateAccess = true))
	FText Name;
};
