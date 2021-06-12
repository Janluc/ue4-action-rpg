// Fill out your copyright notice in the Description page of Project Settings.


#include "MagicAttackNotify.h"

void UMagicAttackNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Player = Cast<APlayerCharacter>(MeshComp->GetOwner());

	if (Player)
	{
		Player->InitMagicDamage();
	}
}
