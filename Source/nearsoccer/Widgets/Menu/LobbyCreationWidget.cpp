// Nearsoccer 2022


#include "Widgets/Menu/LobbyCreationWidget.h"

#include "TestGameInstance.h"
#include "GameFramework/PlayerState.h"

bool ULobbyCreationWidget::Initialize()
{
	bool success = Super::Initialize();
	if (!success || CreateButton == nullptr) return false;

	CreateButton->OnClicked.AddDynamic(this, &ULobbyCreationWidget::OnCreateButtonClicked);
	for (auto& Level : LevelNames)
	{
		LevelComboBox->AddOption(Level.Key);
	}
	LevelComboBox->RefreshOptions();
	LevelComboBox->SetSelectedIndex(0);
	return success;
}

void ULobbyCreationWidget::OnCreateButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("ButtonClicked"));
	auto LobbyName = LobbyBox->Text.ToString();
	if (LobbyName.Len() < 1)
	{
		LobbyName = "Lobby" + FString::FromInt(FMath::RandRange(0, 1e6));
	}
	FString SelectedLevel = LevelComboBox->GetSelectedOption();
	Cast<UTestGameInstance>(GetGameInstance())->Host(LobbyName, SelectedLevel, LevelNames[SelectedLevel]);
}
