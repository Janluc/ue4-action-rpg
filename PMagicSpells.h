// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PMagicSpells.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum EPMagicSpell
{
	Thunder		UMETA(DisplayName = "Thunder"),
	Fire		UMETA(DisplayName = "Fire"),
	Ice			UMETA(DisplayName = "Ice"),
	Wind		UMETA(DisplayName = "Wind")
};