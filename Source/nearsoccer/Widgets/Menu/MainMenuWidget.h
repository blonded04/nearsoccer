// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "MainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class NEARSOCCER_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UEditableTextBox* UsernameBox;

public:
	UFUNCTION(BlueprintCallable)
	void OnSaveNameButtonClicked();
};
