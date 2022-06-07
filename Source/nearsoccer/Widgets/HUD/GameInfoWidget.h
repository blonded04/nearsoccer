// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "NearsoccerCore.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/RichTextBlock.h"
#include "Components/VerticalBox.h"
#include "Engine/Font.h"
#include "GameInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class NEARSOCCER_API UGameInfoWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KillFeed")
	TSubclassOf<URichTextBlock> TextBlockClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Time")
	int INFO_TIME;

	UPROPERTY(EditDefaultsOnly, Category = "Time")
	int ERROR_TIME;
	
	UPROPERTY(EditDefaultsOnly, Category = "Time")
	int KILLFEED_TIME;

	UFUNCTION()
	void Reset();
	
	UFUNCTION()
	void DisableErrorMessage();
	FTimerHandle ErrorTimerHandle;

	UFUNCTION()
	void DisableInfoMessage();
	FTimerHandle InfoTimerHandle;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BlueScoreText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RedScoreText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TimeText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ErrorMessage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* InfoMessage;
	
	UPROPERTY(meta = (BindWidget))
	URichTextBlock* WinnerText;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* KillFeed;

public:
	UFUNCTION(BlueprintCallable)
	void ChangeScoreText(ETeam Team, int NewScore);

	UFUNCTION(BlueprintCallable)
	void ChangeTimeText(int TimeLeft);

	UFUNCTION(BlueprintCallable)
	void SetErrorMessage(const FString& Message);

	UFUNCTION(BlueprintCallable)
	void SetInfoMessage(const FString& Message);

	UFUNCTION(BlueprintCallable)
	void PushToKillFeed(const FString& Killer, const FString& Killed, EDeathType DeathType, bool bWasKilled=false);

	UFUNCTION(BlueprintCallable)
	void SetWinner(EWinnerType Winner);
};

