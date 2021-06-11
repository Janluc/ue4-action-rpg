// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PlayerCharacter.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "BackupHitBoxNotify.generated.h"

/**
 * 
 */

// This Notify is for hit detection during low frames
UCLASS()
class MYPROJECT_API UBackupHitBoxNotify : public UAnimNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	APlayerCharacter* Player;
};
