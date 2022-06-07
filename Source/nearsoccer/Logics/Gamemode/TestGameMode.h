// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TestGameMode.generated.h"

class ANearPowerUp;

UCLASS()
class NEARSOCCER_API ATestGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATestGameMode();

	/**
	 * @brief overriden function - do not change or delete!!!!!!!!
	 */
	virtual FString InitNewPlayer
	(
		APlayerController* NewPlayerController,
		const FUniqueNetIdRepl& UniqueId,
		const FString& Options,
		const FString& Portal
		) override;
	
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	
	virtual void BeginPlay() override;
	
	virtual AActor *ChoosePlayerStart_Implementation(AController* Player) override;
	
	virtual void RestartPlayer(AController* NewPlayer) override;

protected:
	UPROPERTY(EditAnywhere, Category="Respawn")
	float DefaultRespawnTimer{0.25f};
	
	UPROPERTY(EditAnywhere, Category="Respawn")
	float RespawnTimer{DefaultRespawnTimer};
	
	UPROPERTY(EditAnywhere, Category="Buffs")
	TSubclassOf<class UBaseBuff> DefaultBuff;
	
	UPROPERTY()
	TArray<APlayerController*> AllPlayersInGame;

	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	virtual void Logout(AController* Exiting) override;
	
public:
	
	void Respawn(AController* Controller);

	float GetRespawnTimer();
	
	TArray<APlayerController*> GetAllPlayers();

protected:
	int BlueNum = 0;
	
};
