// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PAttackType.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum EPAttackType
{
	Light		UMETA(DisplayName = "Light"),
	Medium		UMETA(DisplayName = "Medium"),
	Heavy		UMETA(DisplayName = "Heavy")
};