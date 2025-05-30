// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/RPGGameplayTags.h"

namespace RPGGameplayTags::Combat
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Data_Damage, "Combat.Data.Damage", "Set By Caller Data Tag For Combat");
}

namespace RPGGameplayTags::Player::State
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Crouch, "Player.State.Crouch", "Player Is Actively Crouching");	
}
