// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CooldownWidget.generated.h"

/**
 * 
 */
UCLASS()
class NEARSOCCER_API UCooldownWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UFUNCTION(BlueprintCallable)
	void SetDashCooldown();

	UPROPERTY()
	class UDashComponent* Component;

	// Check whether we already casted to our player
	bool bCasted{false};

	float Progress{0.0f};
	
protected:
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UProgressBar* DashCooldownBar;
};
