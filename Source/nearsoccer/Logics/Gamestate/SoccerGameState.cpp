// Nearsoccer 2022


#include "Logics/Gamestate/SoccerGameState.h"

EWinnerType ASoccerGameState::CheckWinner(bool bCanReturnNone)
{
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

void ASoccerGameState::ServerChangeBallScore_Implementation(ETeam ScoredTeamType, int DeltaScore)
{
	auto* Team = TeamsByIndexes[ScoredTeamType];
	Team->ChangeScore(DeltaScore);
	MulticastChangeScore(ScoredTeamType, Team->GetScore());
}
