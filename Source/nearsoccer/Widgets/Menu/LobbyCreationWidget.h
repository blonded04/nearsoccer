// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "LobbyCreationWidget.generated.h"

/**
 * 
 */
UCLASS()
class NEARSOCCER_API ULobbyCreationWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual bool Initialize() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UEditableTextBox* LobbyBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UButton* CreateButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UComboBoxString* LevelComboBox;
	
	UPROPERTY(EditDefaultsOnly)
	TMap<FString, FString> LevelNames;
public:
	UFUNCTION(BlueprintCallable)
	void OnCreateButtonClicked();
};
