// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PlayerCharacter.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "MidAttackNotify.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMidAttackNotify : public UAnimNotify
{
	GENERATED_BODY()
	public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	APlayerCharacter* Player;
};
