// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "PlayerStatsComponent.h"
#include "TeamComponent.h"
#include "GameFramework/PlayerState.h"
#include "TestPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class NEARSOCCER_API ATestPlayerState : public APlayerState
{
	GENERATED_BODY()

	ATestPlayerState();
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:
	UPROPERTY(Replicated)
	UTeamComponent* Team;

	UFUNCTION(BlueprintCallable)
	ETeam GetTeamType();

	UFUNCTION(BlueprintCallable)
	int GetTeamPlayersCount();

	UFUNCTION(BlueprintCallable)
	int GetTeamId();

	UFUNCTION(Server, Reliable)
	void SetLastPushed(ATestPlayerState* Pusher);

	UFUNCTION(BlueprintCallable)
	ATestPlayerState* GetLastPushed();

	UFUNCTION(BlueprintCallable)
	FLinearColor GetTeamColor();

	UFUNCTION(Server, Reliable)
	void ModifyStat(EStatisticsType StatType, int Delta);

	float GetStat(EStatisticsType StatType) const;
	float GetPlayerScore() const;

private:
	UPROPERTY(Replicated)
	UPlayerStatsComponent* Stats;

	void ResetLastPushed() { LastPushed = nullptr; }

	// Handle responsible to time after LastPushed resets
	FTimerHandle HandlePusherRemainsAfterPush;

	UPROPERTY(Replicated)
	ATestPlayerState* LastPushed = nullptr;
};
