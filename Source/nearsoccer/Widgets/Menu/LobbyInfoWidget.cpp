// Nearsoccer 2022


#include "Widgets/Menu/LobbyInfoWidget.h"

#include "Components/TextBlock.h"

void ULobbyInfoWidget::Update(int Index, FOnlineSessionSearchResult& Session)
{
	SessionId = Index - 1;

	FString NewSessionName;
	Session.Session.SessionSettings.Get(FName(TEXT("NEARSOCCER_SERVER_NAME_KEY")), NewSessionName);
	SessionName = NewSessionName;

	FString LevelName;
	Session.Session.SessionSettings.Get(FName(TEXT("NEARSOCCER_DISPLAY_LEVEL_NAME_KEY")), LevelName);

	int MaxPlayerCount = Session.Session.SessionSettings.NumPublicConnections;
	int CurrentPlayerCount = MaxPlayerCount - Session.Session.NumOpenPublicConnections;

	IndexText->SetText(FText::AsNumber(Index));
	LobbyNameText->SetText(FText::FromString(NewSessionName));
	MapNameText->SetText(FText::FromString(LevelName));
	PlayersCountText->SetText(FText::AsNumber(CurrentPlayerCount));
	MaxPlayersCountText->SetText(FText::AsNumber(MaxPlayerCount));
	PingText->SetText(FText::AsNumber(Session.PingInMs));
}
