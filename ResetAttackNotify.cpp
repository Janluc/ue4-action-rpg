// Fill out your copyright notice in the Description page of Project Settings.


#include "ResetAttackNotify.h"

void UResetAttackNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Player = Cast<APlayerCharacter>(MeshComp->GetOwner());
	if (Player)
		Player->ResetAttack();
}