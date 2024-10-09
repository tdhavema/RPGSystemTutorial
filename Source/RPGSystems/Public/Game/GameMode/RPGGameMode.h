// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "RPGGameMode.generated.h"

class UProjectileInfo;
class UCharacterClassInfo;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API ARPGGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	UCharacterClassInfo* GetCharacterClassDefaultInfo() const;
	UProjectileInfo* GetProjectileInfo() const;

private:

	UPROPERTY(EditDefaultsOnly, Category= "Custom Values|Class Defaults")
	TObjectPtr<UCharacterClassInfo> ClassDefaults;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Projectiles")
	TObjectPtr<UProjectileInfo> ProjectileInfo;
};
