// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "Logics/Gamestate/TestGameState.h"
#include "DeathmatchGameState.generated.h"

/**
 * 
 */
UCLASS()
class NEARSOCCER_API ADeathmatchGameState : public ATestGameState
{
	GENERATED_BODY()
protected:
	virtual EWinnerType CheckWinner(bool bCanReturnNone=true) override;
public:
	UFUNCTION(Server, Reliable)
	virtual void ServerOnPlayerDied(ETeam PlayerTeam) override;
};
