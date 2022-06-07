// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "Logics/Gamestate/TestGameState.h"
#include "SoccerGameState.generated.h"

/**
 * 
 */
UCLASS()
class NEARSOCCER_API ASoccerGameState : public ATestGameState
{
	GENERATED_BODY()
protected:
	virtual EWinnerType CheckWinner(bool bCanReturnNone=true) override;
public:
	UFUNCTION(Server, Reliable)
	virtual void ServerChangeBallScore(ETeam ScoredTeamType, int DeltaScore) override;
	
};
