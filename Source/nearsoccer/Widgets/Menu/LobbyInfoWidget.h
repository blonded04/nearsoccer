// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h"
#include "LobbyInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class NEARSOCCER_API ULobbyInfoWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* IndexText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* LobbyNameText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* MapNameText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* PlayersCountText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* MaxPlayersCountText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* PingText;

	void Update(int Index, FOnlineSessionSearchResult& Session);
	
	// FOnlineSessionSearchResult SessionInfo;
	UPROPERTY(BlueprintReadWrite)
	int SessionId;

	UPROPERTY(BlueprintReadWrite)
	FString SessionName;
};
