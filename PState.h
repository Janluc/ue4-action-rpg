// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pstate.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum EPState
{
	Idle		UMETA(DisplayName = "Idle"),
	Walking		UMETA(DisplayName = "Walking"),
	Jumping		UMETA(DisplayName = "Jumping"),
	Attacking	UMETA(DisplayName = "Attacking"),
	Dashing		UMETA(DisplayName = "Dashing"),
	Casting		UMETA(DisplayName = "Casting")
};