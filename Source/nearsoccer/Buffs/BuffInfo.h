// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "BuffInfo.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct NEARSOCCER_API FBuffInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString BuffName{"No"};

	UPROPERTY(BlueprintReadOnly)
	UTexture* BuffImage;

	UPROPERTY(BlueprintReadOnly)
	float FXRadius{50.0f};

	UPROPERTY(BlueprintReadOnly)
	float BuffTime{5.0f};

	UPROPERTY(BlueprintReadOnly)
	FLinearColor FXColor;
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsLeftRight;
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsSpawnCircleEffect;
	
	FBuffInfo();

	explicit FBuffInfo(const class UBaseBuff* Buff);
	
};
