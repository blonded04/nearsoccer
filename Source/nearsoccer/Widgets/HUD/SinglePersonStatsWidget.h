// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "TestPlayerState.h"
#include "Blueprint/UserWidget.h"
#include "SinglePersonStatsWidget.generated.h"

/**
 * 
 */
UCLASS()
class NEARSOCCER_API USinglePersonStatsWidget : public UUserWidget
{
	GENERATED_BODY()

	void NativeConstruct() override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UHorizontalBox* StatsBox;

	UFUNCTION(BlueprintCallable)
	void Update(APlayerState* PlayerState, const TArray<TEnumAsByte<EStatisticsType>>& DisplayableStats);

private:
	float score;
};
