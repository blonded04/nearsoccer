#include "Logics/Multiplayer/TestGameInstance.h"

#include "Widgets/Menu/LobbyListWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"

UTestGameInstance::UTestGameInstance()
{
	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(
		this, &UTestGameInstance::OnDestroySessionComplete);
}

void UTestGameInstance::Init()
{
	Super::Init();

	if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(
				this, &UTestGameInstance::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(
				this, &UTestGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.
			                  AddUObject(this, &UTestGameInstance::OnJoinSessionComplete);
		}
	}
}

void UTestGameInstance::OnCreateSessionComplete(FName SessionName, bool Succeeded)
{
	if (Succeeded)
	{
		PrepareController(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		GetWorld()->ServerTravel(m_LevelPath + "?listen"); // TODO: change to const
	}
}

void UTestGameInstance::OnFindSessionComplete(bool Succeeded)
{
	if (Succeeded)
	{
		TArray<FOnlineSessionSearchResult>& SearchResults = SessionSearch->SearchResults;
		if (LobbyListWidget)
		{
			LobbyListWidget->Draw(SearchResults);
		}
	}
}

void UTestGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		FString JoinAddress = "";
		SessionInterface->GetResolvedConnectString(SessionName, JoinAddress);
		if (JoinAddress != "")
		{
			PrepareController(PController);
			PController->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
		}
	}
}

void UTestGameInstance::ListLobbies(ULobbyListWidget* LobbyList)
{
	LobbyListWidget = LobbyList;
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->QuerySettings.Set("SEARCH_PRESENCE", true, EOnlineComparisonOp::Equals);

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UTestGameInstance::JoinLobby(int Id, FString SessionName)
{
	if (Id < SessionSearch->SearchResults.Num())
	{
		if (SessionSearch->SearchResults[Id].IsValid())
		{
			SessionInterface->JoinSession(0, GameSessionName, SessionSearch->SearchResults[Id]);
		}
	}
}

void UTestGameInstance::Host(FString LobbyName, FString DisplayLevelName, FString LevelPath)
{
	UE_LOG(LogTemp, Warning, TEXT("HostServer, username"));
	// FOnlineSessionSettings SessionSettings;

	SessionSettings = MakeShareable(new FOnlineSessionSettings());
	SessionSettings->bAllowJoinInProgress = false;
	SessionSettings->bIsDedicated = false;
	SessionSettings->bIsLANMatch = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bUsesPresence = true;
	SessionSettings->NumPublicConnections = 8;
	SessionSettings->NumPrivateConnections = 0;
	SessionSettings->Set(FName(TEXT("NEARSOCCER_SERVER_NAME_KEY")), LobbyName,
	                     EOnlineDataAdvertisementType::ViaOnlineService);
	SessionSettings->Set(FName(TEXT("NEARSOCCER_DISPLAY_LEVEL_NAME_KEY")), DisplayLevelName,
						 EOnlineDataAdvertisementType::ViaOnlineService);
	m_LevelPath = LevelPath;
	SessionInterface->CreateSession(0, GameSessionName, *SessionSettings);
}

void UTestGameInstance::MarkStarted()
{
	if (SessionInterface.IsValid())
	{
		bool result = SessionInterface->StartSession(GameSessionName);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,
		                                 TEXT("Session started " + FString::FromInt(result)));
	}
}

void UTestGameInstance::Leave()
{
	if (SessionInterface.IsValid())
	{
		bool result = SessionInterface->DestroySession(GameSessionName);

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,
		                                 TEXT("Unreg player " + FString::FromInt(result)));
		if (result)
		{
			UGameplayStatics::OpenLevel(GetWorld(), MainMenuMap, true);
		}
	}
}


void UTestGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
			if (bWasSuccessful)
			{
				UGameplayStatics::OpenLevel(GetWorld(), MainMenuMap, true);
			}
		}
	}
}


void UTestGameInstance::PrepareController(APlayerController* PlayerController)
{
	if (PlayerController)
	{
		PlayerController->SetShowMouseCursor(false);
		PlayerController->SetInputMode(FInputModeGameOnly());
	}
}
