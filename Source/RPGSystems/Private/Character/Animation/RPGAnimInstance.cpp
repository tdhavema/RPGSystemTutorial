// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/RPGAnimInstance.h"

void URPGAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	check(ASC);

	PropertyMap.Initialize(this, ASC);
}
