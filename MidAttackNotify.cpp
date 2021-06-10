// Fill out your copyright notice in the Description page of Project Settings.


#include "MidAttackNotify.h"

void UMidAttackNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Player = Cast<APlayerCharacter>(MeshComp->GetOwner());
	if(Player)
		Player->MidAttack();
	
}
