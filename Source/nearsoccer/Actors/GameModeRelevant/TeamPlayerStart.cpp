// Nearsoccer 2022


#include "Actors/GameModeRelevant/TeamPlayerStart.h"

ETeam ATeamPlayerStart::GetTeam()
{
	return Team;
}

EStartType ATeamPlayerStart::GetType()
{
	return StartType;
}

void ATeamPlayerStart::SetType(EStartType Type)
{
	StartType = Type;
}
