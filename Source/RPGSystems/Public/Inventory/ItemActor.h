// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Equipment/EquipmentTypes.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractInterface.h"
#include "ItemActor.generated.h"

struct FRPGInventoryEntry;

UCLASS()
class RPGSYSTEMS_API AItemActor : public AActor, public IInteractInterface
{
	GENERATED_BODY()

public:

	AItemActor();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void SetParams(const FRPGInventoryEntry* Entry, int32 InNumItems);
	void SetMesh(UStaticMesh* InMesh);

	UPROPERTY(Replicated, BlueprintReadOnly)
	FGameplayTag ItemTag;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 NumItems = 1;

	UPROPERTY(Replicated)
	uint8 ValidationBits = 0;

	UPROPERTY(Replicated, BlueprintReadOnly)
	FEquipmentEffectPackage EffectPackage;

private:

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;

};
