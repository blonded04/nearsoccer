// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TeamComponent.h"
#include "TeamPlayerStart.h"
#include "WallRunnerCharacter.h"
#include "TestGameState.generated.h"

UCLASS()
class NEARSOCCER_API ATestGameState : public AGameStateBase
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION(Server, Reliable)
	void StartSpawnTimer();

	UFUNCTION(Server, Reliable)
	void StopSpawnTimer();

	UFUNCTION()
	void SpawnTimerTick();

	UFUNCTION()
	void ApplySpawnTimerValues(int Value);

	FTimerHandle GameTickHandle;
	void GameTick();

	virtual EWinnerType CheckWinner(bool bCanReturnNone = true) { return EWinnerType::ENone; };

	void EndGame(EWinnerType Winner);

public:
	ATestGameState();

	UFUNCTION(Server, Reliable)
	void AssignPlayerToTeam(APlayerController* NewPlayer);
	UFUNCTION(Server, Reliable)
	void SetPlayerName(APlayerController* NewPlayer);
	UFUNCTION(Server, Reliable)
	void ChangePlayerTeam(APlayerController* Player);
	UFUNCTION(Server, Reliable)
	void ApplyPlayerStencil(APlayerController* Player);
	UFUNCTION(Server, Reliable)
	void InitPlayerStencils();

	UFUNCTION(Server, Reliable)
	void RebuildScoreboard();

	UFUNCTION(Server, Reliable)
	void RebuildInfoWidgets(AWallRunnerCharacter* Character);
	UFUNCTION(Server, Reliable)
	void UpdateKillFeed(const FString& Killer, const FString& Killed, EDeathType DeathType, bool bWasKilled = false);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayWinningVideo(EWinnerType Winner);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRebuildScoreboard();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastUpdateKillFeed(const FString& Killer, const FString& Killed, EDeathType DeathType,
	                             bool bWasKilled = false);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetWinner(EWinnerType Winner);

	UFUNCTION(NetMulticast, Reliable)
	void TimeTick();

	virtual void ServerChangeBallScore(ETeam ScoredTeamType, int DeltaScore)
	{
	};

	virtual void ServerOnPlayerDied(ETeam PlayerTeam)
	{
	};

	UFUNCTION(NetMulticast, Reliable)
	void MulticastChangeScore(ETeam ScoredTeamType, int NewScore);

	UFUNCTION(Server, Reliable)
	void MovePlayersToSpawns(EStartType SpawnType);

	UFUNCTION(Server, Reliable)
	void MovePlayerToSpawnPoint(AWallRunnerCharacter* Character, EStartType StartType, ETeam TeamType);

	UFUNCTION(Server, Reliable)
	void ChangePlayerName(APlayerState* PlayerState, const FString& Name);

	UFUNCTION()
	void BeginGame();

	UFUNCTION()
	int GetLastPushedResetTime() { return RESET_LAST_PUSHED_TIME; };

	UFUNCTION(Server, Reliable)
	void DestroyServer();

	UFUNCTION(Server, Reliable)
	void OnTogglePlayerReady(APlayerState* Player, bool IsReady);

	TArray<TEnumAsByte<EStatisticsType>> GetDisplayableStats();

	EWinnerType GetWinner() { return GameWinner; };

	bool IsGameRunning();

	EStartType GetCurrentStartType() { return CurrentStart; };
	EGameState GetCurrentGameState() { return CurrentGameState; };

	UPROPERTY(Replicated)
	UTeamComponent* BlueTeam;
	UPROPERTY(Replicated)
	UTeamComponent* RedTeam;
protected:
	TMap<ETeam, UTeamComponent*> TeamsByIndexes{{ETeam::EBlue, BlueTeam}, {ETeam::ERed, RedTeam}};
	
	
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Displayable stats")
	TArray<TEnumAsByte<EStatisticsType>> DEFAULT_DISPLAYABLE_STATS = {KILLS, DEATHS, GOALS_SCORED, SCORE};

	int GameTimeLeft;
	UPROPERTY(EditDefaultsOnly)
	int DEFAULT_TIME_UNTIL_START = 10;
	UPROPERTY(EditDefaultsOnly)
	int DEFAULT_GAME_TIME = 60;
	UPROPERTY(EditDefaultsOnly)
	int TIME_BEFORE_SERVER_SHUTDOWN = 10;
	UPROPERTY(EditDefaultsOnly)
	int RESET_LAST_PUSHED_TIME = 10;
	UPROPERTY(EditDefaultsOnly)
	int DEFAULT_TEAM_SCORE = 0;

	UPROPERTY(EditAnywhere)
	TArray<FString> DefaultPlayerNames{"Pa$hok", "LuTiy", "Kro$$h", "__BIGFLOPPA__", "XyL[i]gaN4eG", "IIo3uTuB4uK"};
private:
	TArray<APlayerController*> Players;
	TMap<FString, int> UsedNames{};

	TSet<APlayerState*> ReadyPlayers;

	FTimerHandle SpawnTimerHandle;
	FTimerHandle GameEndHandle;

	EStartType CurrentStart = EStartType::HUB;

	UPROPERTY(Replicated)
	EWinnerType GameWinner = EWinnerType::ENone;

	UPROPERTY(Replicated)
	EGameState CurrentGameState = EGameState::EHub;

	int TimeUntilStart = DEFAULT_TIME_UNTIL_START;

	UPROPERTY(Replicated)
	bool GameRunning = false;
};
