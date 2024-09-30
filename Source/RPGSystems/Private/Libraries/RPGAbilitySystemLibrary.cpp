// Fill out your copyright notice in the Description page of Project Settings.


#include "Libraries/RPGAbilitySystemLibrary.h"

#include "Game/GameMode/RPGGameMode.h"
#include "Kismet/GameplayStatics.h"

UCharacterClassInfo* URPGAbilitySystemLibrary::GetCharacterClassDefaultInfo(const UObject* WorldContextObject)
{
	if (const ARPGGameMode* RPGGameMode = Cast<ARPGGameMode>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		return RPGGameMode->GetCharacterClassDefaultInfo();
	}

	return nullptr;
}

UProjectileInfo* URPGAbilitySystemLibrary::GetProjectileInfo(const UObject* WorldContextObject)
{
	if (const ARPGGameMode* RPGGameMode = Cast<ARPGGameMode>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		return RPGGameMode->GetProjectileInfo();
	}

	return nullptr;
}
