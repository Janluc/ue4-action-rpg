// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PlayerCharacter.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "ResetMovementNotify.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UResetMovementNotify : public UAnimNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	APlayerCharacter* Player;
};
