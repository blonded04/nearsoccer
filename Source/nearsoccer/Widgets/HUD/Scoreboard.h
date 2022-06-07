// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "SinglePersonStatsWidget.h"
#include "Components/WidgetComponent.h"
#include "Scoreboard.generated.h"

/**
 * 
 */
UCLASS()
class NEARSOCCER_API UScoreboard : public UUserWidget
{
	GENERATED_BODY()

	void NativeConstruct() override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UVerticalBox* BlueBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UVerticalBox* RedBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* RedStatus;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* BlueStatus;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Person stats")
	TSubclassOf<class USinglePersonStatsWidget> SinglePersonStatsClass;

	UFUNCTION(BlueprintCallable)
	void Update();

private:
	void Generate();

	void GenerateSingleTeam(UTeamComponent* Team, const TArray<TEnumAsByte<EStatisticsType>>& DisplayableStats);
};
