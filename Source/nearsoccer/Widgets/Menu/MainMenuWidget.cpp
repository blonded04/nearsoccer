// Nearsoccer 2022


#include "Widgets/Menu/MainMenuWidget.h"

#include "GameFramework/PlayerState.h"

void UMainMenuWidget::OnSaveNameButtonClicked()
{
	auto Username = UsernameBox->Text.ToString();
	if (Username.Len() < 1)
	{
		return;
	}
	GetWorld()->GetFirstPlayerController()->GetPlayerState<APlayerState>()->SetPlayerName(Username);
}
