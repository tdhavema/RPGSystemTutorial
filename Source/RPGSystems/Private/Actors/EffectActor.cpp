// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/EffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/BoxComponent.h"

AEffectActor::AEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	ActorMesh = CreateDefaultSubobject<UStaticMeshComponent>("ActorMesh");
	SetRootComponent(ActorMesh);

	OverlapBox = CreateDefaultSubobject<UBoxComponent>("OverlapBox");
	OverlapBox->SetupAttachment(GetRootComponent());
	OverlapBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

}

void AEffectActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		OverlapBox->OnComponentBeginOverlap.AddDynamic(this, &AEffectActor::OnBoxBeginOverlap);
	}
	
}

void AEffectActor::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (UAbilitySystemComponent* OtherASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
	{
		const FGameplayEffectContextHandle ContextHandle = OtherASC->MakeEffectContext();
		const FGameplayEffectSpecHandle SpecHandle = OtherASC->MakeOutgoingSpec(OverlapEffect, 1.f, ContextHandle);
		OtherASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		Destroy();
	}
}


