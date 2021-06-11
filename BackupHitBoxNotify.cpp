// Fill out your copyright notice in the Description page of Project Settings.


#include "BackupHitBoxNotify.h"

void UBackupHitBoxNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Player = Cast<APlayerCharacter>(MeshComp->GetOwner());

	if (Player)
	{
		Player->BackupHitBox();
	}
}
