// Nearsoccer 2022


#include "Scoreboard.h"

#include "TestGameState.h"
#include "TestPlayerState.h"
#include "Components/VerticalBox.h"

void UScoreboard::Update()
{
	Generate();
}

void UScoreboard::Generate()
{
	auto GState = GetWorld()->GetGameState<ATestGameState>();
	UE_LOG(LogTemp, Warning, TEXT("Team %d has %d players"), 0, GState->BlueTeam->GetPlayersCount());
	auto DisplayableStats = GState->GetDisplayableStats();
	if (GState->GetCurrentGameState() != EGameState::EHub)
	{
		BlueStatus->RemoveFromParent();
		RedStatus->RemoveFromParent();
	}
	GenerateSingleTeam(GState->BlueTeam, DisplayableStats);
	GenerateSingleTeam(GState->RedTeam, DisplayableStats);
}


void UScoreboard::GenerateSingleTeam(UTeamComponent* Team, const TArray<TEnumAsByte<EStatisticsType>>& DisplayableStats)
{
	UVerticalBox* TeamBox = BlueBox;
	UE_LOG(LogTemp, Warning, TEXT("Team %d has %d players"), Team->GetTeamType(), Team->GetPlayersCount());
	if (Team->GetTeamType() == ETeam::ERed)
	{
		TeamBox = RedBox;
	}

	TeamBox->ClearChildren();

	auto players = Team->GetPlayers();
	Algo::Sort(players, [](APlayerState* lhs, APlayerState* rhs)
	{
		if (lhs == nullptr)
			return false;
		if (rhs == nullptr)
			return true;
		if (Cast<ATestPlayerState>(lhs)->GetPlayerScore() == Cast<ATestPlayerState>(rhs)->GetPlayerScore())
		{
			return Cast<ATestPlayerState>(lhs)->GetPlayerName() < Cast<ATestPlayerState>(rhs)->GetPlayerName();
		}
		return Cast<ATestPlayerState>(lhs)->GetPlayerScore() > Cast<ATestPlayerState>(rhs)->GetPlayerScore();
	});

	for (auto Player : players)
	{
		if (!IsValid(Player))
		{
			continue;;
		}
		auto* widget = CreateWidget<USinglePersonStatsWidget>(this, SinglePersonStatsClass);
		widget->Update(Player, DisplayableStats);
		TeamBox->AddChild(widget);
	}
}

void UScoreboard::NativeConstruct()
{
	// UE_LOG(LogTemp, Warning, TEXT("SCOREBOARD NATIVE CONSTRUCT"));
	Super::NativeConstruct();
	Generate();
}
