// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "TestPlayerState.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "TeamSelectWidget.generated.h"

/**
 * 
 */
UCLASS()
class NEARSOCCER_API UTeamSelectWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	void Draw(ETeam CurrentTeam = ETeam::ENone);
	virtual bool Initialize() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UEditableTextBox* NameBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* JoinRed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* RedButtonText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* JoinBlue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* BlueButtonText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* SaveName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* BlueTeamCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* RedTeamCount;

private:
	UFUNCTION()
	void OnJoinButtonClicked();

	UFUNCTION()
	void OnSaveNameClicked();
};
