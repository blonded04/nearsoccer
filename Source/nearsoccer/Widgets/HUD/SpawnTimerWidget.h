// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "SpawnTimerWidget.generated.h"

/**
 * 
 */
UCLASS()
class NEARSOCCER_API USpawnTimerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* TimerText;

	void SetTimeLeft(int TimeLeft);
	
};
