// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractInterface.h"
#include "NPCBase.generated.h"

UCLASS()
class RPGSYSTEMS_API ANPCBase : public AActor, public IInteractInterface
{
	GENERATED_BODY()

public:
	
	ANPCBase();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Custom Values|Name")
	FText Name;

private:

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> Mesh;

};
