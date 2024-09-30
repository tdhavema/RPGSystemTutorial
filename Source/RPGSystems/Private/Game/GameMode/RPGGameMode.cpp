// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GameMode/RPGGameMode.h"

UCharacterClassInfo* ARPGGameMode::GetCharacterClassDefaultInfo() const
{
	return ClassDefaults;
}

UProjectileInfo* ARPGGameMode::GetProjectileInfo() const
{
	return ProjectileInfo;
}
