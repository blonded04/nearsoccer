// Nearsoccer 2022


#include "TestPlayerState.h"

#include "TestGameState.h"
#include "Net/UnrealNetwork.h"

void ATestPlayerState::ModifyStat_Implementation(EStatisticsType StatType, int Delta)
{
	if (HasAuthority())
	{
		if (Stats->ModifyStat(StatType, Delta))
		{
			Cast<ATestGameState>(GetWorld()->GetGameState())->RebuildScoreboard();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Trying to change player statistic, but have no authority"));
	}
}

ATestPlayerState::ATestPlayerState() : Super()
{
	Stats = CreateDefaultSubobject<UPlayerStatsComponent>("Stats component");
	if (Stats)
	{
		Stats->SetNetAddressable();
		Stats->SetIsReplicated(true);
	}

	Team = CreateDefaultSubobject<UTeamComponent>("Team component");
	if (Team)
	{
		Team->SetNetAddressable();
		Team->SetIsReplicated(true);
	}
}

void ATestPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void ATestPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATestPlayerState, Team);
	DOREPLIFETIME(ATestPlayerState, LastPushed);
}

ETeam ATestPlayerState::GetTeamType()
{
	return Team->GetTeamType();
}

int ATestPlayerState::GetTeamPlayersCount()
{
	return Team->GetPlayersCount();
}

int ATestPlayerState::GetTeamId()
{
	return Team->GetTeamId();
}

void ATestPlayerState::SetLastPushed_Implementation(ATestPlayerState* Pusher)
{
	
	GetWorldTimerManager().ClearTimer(HandlePusherRemainsAfterPush);
	GetWorldTimerManager().SetTimer(HandlePusherRemainsAfterPush, this, &ATestPlayerState::ResetLastPushed,
	                                Cast<ATestGameState>(GetWorld()->GetGameState())->GetLastPushedResetTime());
	LastPushed = Pusher;
}

ATestPlayerState* ATestPlayerState::GetLastPushed()
{
	return LastPushed;
}

FLinearColor ATestPlayerState::GetTeamColor()
{
	return Team->GetColor();
}

float ATestPlayerState::GetStat(EStatisticsType StatType) const
{
	return Stats->GetStat(StatType);
}

float ATestPlayerState::GetPlayerScore() const
{
	return Stats->GetStat(SCORE);
}
