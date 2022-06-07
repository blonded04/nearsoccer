// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "LobbyInfoWidget.h"
#include "Components/VerticalBox.h"
#include "LobbyListWidget.generated.h"

/**
 * 
 */
UCLASS()
class NEARSOCCER_API ULobbyListWidget : public UUserWidget
{
	GENERATED_BODY()

	void NativeConstruct() override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UVerticalBox* LobbyBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* LoadingText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lobby info")
	TSubclassOf<class ULobbyInfoWidget> LobbyInfoClass;

	void Draw(TArray<FOnlineSessionSearchResult>& Lobbies);

	UFUNCTION(BlueprintCallable)
	void Update();
	
	UPROPERTY(EditDefaultsOnly)
	float SpinRate;
private:

	void LoadingTick();
	FTimerHandle LoadingTimerHandler;
};
