// Fill out your copyright notice in the Description page of Project Settings.


#include "HitBoxOffNotify.h"

void UHitBoxOffNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Player = Cast<APlayerCharacter>(MeshComp->GetOwner());
	if (Player)
	{
		Player->HitBoxOff();
	}
}
