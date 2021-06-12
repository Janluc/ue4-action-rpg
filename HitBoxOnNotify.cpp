// Fill out your copyright notice in the Description page of Project Settings.


#include "HitBoxOnNotify.h"

void UHitBoxOnNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Player = Cast<APlayerCharacter>(MeshComp->GetOwner());
	if (Player)
	{
		Player->HitBoxOn();
	}
}
