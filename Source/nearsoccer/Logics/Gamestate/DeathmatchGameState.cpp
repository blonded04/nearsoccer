// Nearsoccer 2022


#include "Logics/Gamestate/DeathmatchGameState.h"

EWinnerType ADeathmatchGameState::CheckWinner(bool bCanReturnNone)
{
	// In DM mode Score is num of respawns left for the team.
	if (RedTeam->GetScore() < 0)
	{
		return EWinnerType::EBlue;
	}
	if (BlueTeam->GetScore() < 0)
	{
		return EWinnerType::ERed;
	}
	if (bCanReturnNone)
	{
		return EWinnerType::ENone;
	}
	
	EWinnerType Winner = EWinnerType::EDraw;
	int RedScore = RedTeam->GetScore();
	int BlueScore = BlueTeam->GetScore();
	if (BlueScore > RedScore)
	{
		Winner = EWinnerType::EBlue;
	}
	else if (RedScore > BlueScore)
	{
		Winner = EWinnerType::ERed;
	}
	return Winner;
}

void ADeathmatchGameState::ServerOnPlayerDied_Implementation(ETeam PlayerTeam)
{
	auto* Team = TeamsByIndexes[PlayerTeam];
	UE_LOG(LogTemp, Warning, TEXT("SCORE CHANGING: team score is %d"), Team->GetScore());
	Team->ChangeScore(-1);
	auto Winner = CheckWinner(true);
	
	if (Winner != EWinnerType::ENone)
	{
		EndGame(Winner);
		return;
	}
	
	MulticastChangeScore(PlayerTeam, Team->GetScore());
}
