// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"

/**
 * @brief Check whether pointer is still valid
 * @param obj pointer to object
 */
#define VOID_IS_VALID(obj) if (obj == nullptr || !obj->IsValidLowLevelFast()) { return; }

/**
 * @brief Check whether pointer is still valid
 * @param obj pointer to object
 * @returns val if not valid
 */
#define VALUE_IS_VALID(obj, val) if (obj == nullptr || !obj->IsValidLowLevelFast()) { return val; }

/**
 * @brief Check whether Var == Expected value
 * @param Var - variable to check
 * @param Expected - expected value of Var
 */
#define VOID_CHECK(Var, Expected) if (Var != Expected) { return; }

#define COUNT_STENCIL(Stencil, TeamId, b_UseWH, b_IsPlayer) Stencil = TeamId; if(!b_UseWH) { Stencil += 10; }; if (!b_IsPlayer) { Stencil += 1;}

UENUM(BlueprintType)
enum class ETeam : uint8
{
	ENone = 0 UMETA(DisplayName = "None"),
	EBlue = 1 UMETA(DisplayName = "Blue"),
	ERed = 2 UMETA(DisplayName = "Red")
};

UENUM(BlueprintType)
enum class EGameMode : uint8
{
	ESoccer = 0 UMETA(DisplayName = "Soccer"),
	EDeathMatch=1 UMETA(DisplayName = "Deathmatch")
};

UENUM(BlueprintType)
enum class EGameState : uint8
{
	EHub = 0,
	EInProgress = 1,
	EFinished = 2
};


UENUM(BlueprintType)
enum class EWinnerType : uint8
{
	EDraw = 0,
	EBlue = 1,
	ERed = 2,
	ENone = 3
};

UENUM()
enum EDeathType
{
	EGrenade = 0,
	ELava = 1
};

static FORCEINLINE FText GetFloatAsTextWithPrecision(float TheFloat, int32 Precision,
                                                     bool IncludeLeadingZero = true)
{
	float Rounded = roundf(TheFloat);
	if (FMath::Abs(TheFloat - Rounded) < FMath::Pow(10, -1 * Precision))
	{
		TheFloat = Rounded;
	}
	FNumberFormattingOptions NumberFormat;
	NumberFormat.MinimumIntegralDigits = (IncludeLeadingZero) ? 1 : 0;
	NumberFormat.MaximumIntegralDigits = 10000;
	NumberFormat.MinimumFractionalDigits = Precision;
	NumberFormat.MaximumFractionalDigits = Precision;
	return FText::AsNumber(TheFloat, &NumberFormat);
}
