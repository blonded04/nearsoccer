// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpeedWidget.generated.h"

/**
 * 
 */
UCLASS()
class NEARSOCCER_API USpeedWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
protected:
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* SpeedInfo;
};
