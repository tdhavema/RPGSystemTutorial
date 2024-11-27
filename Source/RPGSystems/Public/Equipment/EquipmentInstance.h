// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EquipmentInstance.generated.h"

/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UEquipmentInstance : public UObject
{
	GENERATED_BODY()

public:

	virtual void OnEquipped();
	virtual void OnUnEquipped();
	
};
