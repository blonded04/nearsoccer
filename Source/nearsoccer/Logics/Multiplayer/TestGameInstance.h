#pragma once

#include "CoreMinimal.h"
#include "Widgets/Menu/LobbyListWidget.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Engine/GameInstance.h"
#include "TestGameInstance.generated.h"

/**
* 
*/
UCLASS()
class NEARSOCCER_API UTestGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UTestGameInstance();

	UFUNCTION(BlueprintCallable)
	void ListLobbies(ULobbyListWidget* LobbyList);

	UFUNCTION(BlueprintCallable)
	void JoinLobby(int Id, FString SessionName);

	UFUNCTION(BlueprintCallable)
	void Host(FString LobbyName, FString DisplayLevelName, FString LevelPath);

	void MarkStarted();

	void Leave();


protected:
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	IOnlineSessionPtr SessionInterface;

	virtual void Init() override;

	virtual void OnCreateSessionComplete(FName SessionName, bool Succeeded);
	virtual void OnFindSessionComplete(bool Succeeded);
	virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	FName LobbyMapName;

	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	FName MainMenuMap;

	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
	virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	TSharedPtr<class FOnlineSessionSettings> SessionSettings;
	ULobbyListWidget* LobbyListWidget;
	void PrepareController(APlayerController* PlayerController);

	FString m_LevelPath;
};
