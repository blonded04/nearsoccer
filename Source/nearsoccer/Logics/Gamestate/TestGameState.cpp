// Nearsoccer 2022


#include "TestGameState.h"

#include "EngineUtils.h"
#include "PlayerStatsComponent.h"
#include "TeamPlayerStart.h"
#include "TestGameInstance.h"
#include "TestPlayerState.h"
#include "WallRunnerController.h"
#include "GameFramework/Character.h"
#include "Character/WallRunnerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

void ATestGameState::BeginPlay()
{
	Super::BeginPlay();
	TeamsByIndexes[ETeam::EBlue]->ChangeScore(DEFAULT_TEAM_SCORE);
	TeamsByIndexes[ETeam::ERed]->ChangeScore(DEFAULT_TEAM_SCORE);
}

void ATestGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATestGameState, BlueTeam);
	DOREPLIFETIME(ATestGameState, RedTeam);
	DOREPLIFETIME(ATestGameState, GameRunning);
	DOREPLIFETIME(ATestGameState, GameWinner);
	DOREPLIFETIME(ATestGameState, CurrentGameState);
}

void ATestGameState::StartSpawnTimer_Implementation()
{
	if (SpawnTimerHandle.IsValid())
	{
		return;
	}
	TimeUntilStart = DEFAULT_TIME_UNTIL_START;
	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this,
	                                       &ATestGameState::SpawnTimerTick, 1, true, 0);
}

void ATestGameState::StopSpawnTimer_Implementation()
{
	if (SpawnTimerHandle.IsValid())
	{
		ApplySpawnTimerValues(-1);
		GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
	}
}

void ATestGameState::SpawnTimerTick()
{
	ApplySpawnTimerValues(TimeUntilStart);
	if (TimeUntilStart <= 0)
	{
		StopSpawnTimer();
		BeginGame();
	}
	TimeUntilStart--;
}

void ATestGameState::ApplySpawnTimerValues(int Value)
{
	for (TActorIterator<AWallRunnerCharacter> It(GetWorld()); It; ++It)
	{
		auto Player = *It;
		if (Player)
		{
			Player->SetSpawnTimerValue(Value);
		}
	}
}

void ATestGameState::GameTick()
{
	TimeTick();
	GameTimeLeft--;
	if (GameTimeLeft <= 0)
	{
		if (GameTickHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(GameTickHandle);
		}
		EndGame(CheckWinner(false));
	}
}

void ATestGameState::EndGame(EWinnerType Winner)
{
	check(Winner != EWinnerType::ENone); // Why do you even end your game if there is no winner?

	GameRunning = false;
	CurrentStart = EStartType::ENDGAME;
	GameWinner = Winner;
	CurrentGameState = EGameState::EFinished;

	GetWorld()->GetTimerManager().ClearTimer(GameTickHandle);

	MulticastSetWinner(Winner);
	MovePlayersToSpawns(EStartType::ENDGAME);
	// PlayWinningVideo(Winner);
	GetWorld()->GetTimerManager().SetTimer(GameEndHandle, this,
	                                       &ATestGameState::DestroyServer, TIME_BEFORE_SERVER_SHUTDOWN, true);
}

ATestGameState::ATestGameState(): Super()
{
	BlueTeam = CreateDefaultSubobject<UTeamComponent>("Blue team component");
	BlueTeam->Init(ETeam::EBlue, FLinearColor(0.3f, 0.3f, 1.0f, 1));
	if (BlueTeam)
	{
		BlueTeam->SetNetAddressable();
		BlueTeam->SetIsReplicated(true);
	}
	UE_LOG(LogTemp, Warning, TEXT("Constructor blue score : %d"), BlueTeam->GetScore());
	TeamsByIndexes[ETeam::EBlue] = BlueTeam;
	UE_LOG(LogTemp, Warning, TEXT("Constructor blue score by index: %d"), TeamsByIndexes[ETeam::EBlue]->GetScore());

	RedTeam = CreateDefaultSubobject<UTeamComponent>("Red team component");
	RedTeam->Init(ETeam::ERed, FLinearColor(1.0f, 0.3f, 0.3f, 1));
	if (RedTeam)
	{
		RedTeam->SetNetAddressable();
		RedTeam->SetIsReplicated(true);
	}
	TeamsByIndexes[ETeam::ERed] = RedTeam;
}

void ATestGameState::RebuildInfoWidgets_Implementation(AWallRunnerCharacter* Character)
{
	if (Character)
	{
		Character->SetGameTimerValue(GameTimeLeft);
		Character->ChangeScoreUI(ETeam::EBlue, TeamsByIndexes[ETeam::EBlue]->GetScore());
		Character->ChangeScoreUI(ETeam::ERed, TeamsByIndexes[ETeam::ERed]->GetScore());
	}
}

void ATestGameState::UpdateKillFeed_Implementation(const FString& Killer, const FString& Killed, EDeathType DeathType,
                                                   bool bWasKilled)
{
	MulticastUpdateKillFeed(Killer, Killed, DeathType, bWasKilled);
}

void ATestGameState::MulticastUpdateKillFeed_Implementation(const FString& Killer, const FString& Killed,
                                                            EDeathType DeathType, bool bWasKilled)
{
	if (auto PlayerCharacter = Cast<AWallRunnerCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
	{
		PlayerCharacter->UpdateKillFeed(
			Killer, Killed, DeathType, bWasKilled);
	}
}


void ATestGameState::MulticastSetWinner_Implementation(EWinnerType Winner)
{
	if (auto PlayerCharacter = Cast<AWallRunnerCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
	{
		PlayerCharacter->SetWinner(Winner);
	}
}

void ATestGameState::InitPlayerStencils_Implementation()
{
	for (TActorIterator<AWallRunnerController> It(GetWorld()); It; ++It)
	{
		auto Player = *It;
		ApplyPlayerStencil(Player);
	}
}

void ATestGameState::RebuildScoreboard_Implementation()
{
	MulticastRebuildScoreboard();
}

void ATestGameState::MulticastRebuildScoreboard_Implementation()
{
	Cast<AWallRunnerCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter())->RebuildScoreboard();
}

void ATestGameState::MovePlayerToSpawnPoint_Implementation(AWallRunnerCharacter* Character, EStartType StartType,
                                                           ETeam TeamType)
{
	UE_LOG(LogTemp, Warning, TEXT("TP function called, num of players %d"), PlayerArray.Num());
	auto PawnToFit = PlayerArray[0]->GetPawn();
	TArray<ATeamPlayerStart*> TeamStartPoints;
	UWorld* World = GetWorld();

	for (TActorIterator<ATeamPlayerStart> It(World); It; ++It)
	{
		ATeamPlayerStart* PlayerStart = *It;
		auto Type = PlayerStart->GetType();
		if (Type != StartType || PlayerStart->GetTeam() != TeamType)
		{
			continue;
		}

		FVector ActorLocation = PlayerStart->GetActorLocation();
		const FRotator ActorRotation = PlayerStart->GetActorRotation();
		if (!World->EncroachingBlockingGeometry(PawnToFit, ActorLocation, ActorRotation))
		{
			auto StartPoint = PlayerStart;
			Character->GetCharacterMovement()->StopMovementImmediately();
			Character->TeleportTo(StartPoint->GetActorLocation(), StartPoint->GetActorRotation());
		}
	}
}


// TODO: maybe slow...
void ATestGameState::TimeTick_Implementation()
{
	for (TActorIterator<AWallRunnerCharacter> It(GetWorld()); It; ++It)
	{
		auto Player = *It;
		if (Player)
		{
			Player->SetGameTimerValue(GameTimeLeft);
		}
	}
}

void ATestGameState::ChangePlayerName_Implementation(APlayerState* PlayerState, const FString& Name)
{
	if (PlayerState)
	{
		PlayerState->SetPlayerName(Name);
		UE_LOG(LogTemp, Warning, TEXT("Changed player name"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PS is null"));
	}
}

void ATestGameState::BeginGame()
{
	GameRunning = true;
	CurrentStart = EStartType::GAMEFIELD;
	CurrentGameState = EGameState::EInProgress;
	if (auto Instance = Cast<UTestGameInstance>(GetGameInstance()))
	{
		Instance->MarkStarted();
	}
	InitPlayerStencils();
	MovePlayersToSpawns(EStartType::GAMEFIELD);

	for (TActorIterator<AWallRunnerCharacter> It(GetWorld()); It; ++It)
	{
		auto Player = *It;
		if (Player)
		{
			Player->ChangeScoreUI(ETeam::EBlue, DEFAULT_TEAM_SCORE);
			Player->ChangeScoreUI(ETeam::ERed, DEFAULT_TEAM_SCORE);
		}
	}

	GameTimeLeft = DEFAULT_GAME_TIME;
	GetWorld()->GetTimerManager().SetTimer(GameTickHandle, this,
	                                       &ATestGameState::GameTick, 1, true, 0);
}

void ATestGameState::DestroyServer_Implementation()
{
	if (auto Instance = Cast<UTestGameInstance>(GetGameInstance()))
	{
		Instance->Leave();
	}
}

TArray<TEnumAsByte<EStatisticsType>> ATestGameState::GetDisplayableStats()
{
	auto Stats = DEFAULT_DISPLAYABLE_STATS;
	if (CurrentGameState == EGameState::EHub)
	{
		Stats.Add(READY_STATUS);
	}
	return Stats;
}

bool ATestGameState::IsGameRunning()
{
	return GameRunning;
}

void ATestGameState::OnTogglePlayerReady_Implementation(APlayerState* Player, bool IsReady)
{
	if (GameRunning)
	{
		return;
	}
	auto PState = Cast<ATestPlayerState>(Player);
	if (IsReady)
	{
		PState->ModifyStat(READY_STATUS, 1);
		ReadyPlayers.Add(Player);
		if (ReadyPlayers.Num() == PlayerArray.Num())
		{
			StartSpawnTimer();
		}
	}
	else
	{
		PState->ModifyStat(READY_STATUS, -1);
		ReadyPlayers.Remove(Player);
		if (ReadyPlayers.Num() != PlayerArray.Num())
		{
			StopSpawnTimer();
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Num ready players %d"), ReadyPlayers.Num());
}

void ATestGameState::MovePlayersToSpawns_Implementation(EStartType SpawnType)
{
	UE_LOG(LogTemp, Warning, TEXT("TP function called, num of players %d"), PlayerArray.Num());
	auto PawnToFit = PlayerArray[0]->GetPawn();
	TMap<ETeam, TArray<ATeamPlayerStart*>> TeamsStartPoints{{ETeam::EBlue, {}}, {ETeam::ERed, {}}};
	UWorld* World = GetWorld();

	for (TActorIterator<ATeamPlayerStart> It(World); It; ++It)
	{
		ATeamPlayerStart* PlayerStart = *It;
		auto Type = PlayerStart->GetType();
		if (Type != SpawnType)
		{
			continue;
		}

		FVector ActorLocation = PlayerStart->GetActorLocation();
		const FRotator ActorRotation = PlayerStart->GetActorRotation();
		if (!World->EncroachingBlockingGeometry(PawnToFit, ActorLocation, ActorRotation))
		{
			TeamsStartPoints[PlayerStart->GetTeam()].Add(PlayerStart);
		}
	}

	for (TActorIterator<AWallRunnerCharacter> It(GetWorld()); It; ++It)
	{
		auto Player = *It;
		if (!Player || !Player->GetPlayerState())
		{
			continue;
		}
		auto team = Cast<ATestPlayerState>(Player->GetPlayerState())->GetTeamType();
		if (TeamsStartPoints[team].Num() == 0)
		{
			continue;
		}

		auto StartPoint = TeamsStartPoints[team].Last();

		Player->GetCharacterMovement()->StopMovementImmediately();
		Player->ResetRotation(StartPoint->GetActorRotation());
		if (HasAuthority())
		{
			UE_LOG(LogTemp, Warning, TEXT("Authority"));
		}
		bool res = Player->TeleportTo(StartPoint->GetActorLocation(), StartPoint->GetActorRotation());

		if (SpawnType == EStartType::ENDGAME)
		{
			Player->GetCharacterMovement()->DisableMovement();
		}

		UE_LOG(LogTemp, Warning, TEXT("TP is success %d"), res);
		if (TeamsStartPoints[team].Num() > 1)
		{
			TeamsStartPoints[team].Pop();
		}
	}
}

void ATestGameState::SetPlayerName_Implementation(APlayerController* NewPlayer)
{
	FString Name = DefaultPlayerNames[FMath::RandRange(0, DefaultPlayerNames.Num() - 1)];
	if (UsedNames.Contains(Name))
	{
		UsedNames[Name]++;
		Name.Append(" (" + FString::FromInt(UsedNames[Name]) + ")");
	}
	else
	{
		UsedNames.Add(Name, 1);
	}
	NewPlayer->SetName(Name);
}

void ATestGameState::AssignPlayerToTeam_Implementation(APlayerController* NewPlayer)
{
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Adding Player To Team"));
		Players.Push(NewPlayer);
		UTeamComponent* team = BlueTeam;
		if (BlueTeam->GetPlayersCount() > RedTeam->GetPlayersCount())
		{
			team = RedTeam;
		}
		UE_LOG(LogTemp, Warning, TEXT("Team choosed"));
		team->AddMember(NewPlayer);

		FString Name = DefaultPlayerNames[FMath::RandRange(0, DefaultPlayerNames.Num() - 1)];

		ATestPlayerState* PState = Cast<ATestPlayerState>(NewPlayer->PlayerState);
		const int TeamIndex = static_cast<uint8>(team->GetTeamType());
		PState->Team = team;
		auto Character = Cast<AWallRunnerCharacter>(NewPlayer->GetCharacter());
		Character->SetTeamId(PState->GetTeamId());

		UE_LOG(LogTemp, Warning, TEXT("Added player with id %u and name %p to team %d"), NewPlayer->GetUniqueID(),
		       TCHAR_TO_UTF8(*NewPlayer->PlayerState->GetPlayerName()), TeamIndex);
	}
}

void ATestGameState::ApplyPlayerStencil_Implementation(APlayerController* Player)
{
	for (TActorIterator<AWallRunnerController> It(GetWorld()); It; ++It)
	{
		auto OtherPlayer = *It;
		if (OtherPlayer == nullptr || Player == OtherPlayer || OtherPlayer->GetCharacter() == nullptr || Player->
			GetCharacter() == nullptr)
			continue;
		int OtherStencil, SelfStencil;
		int OtherTeamId = Cast<ATestPlayerState>(OtherPlayer->PlayerState)->GetTeamId();
		int SelfTeamId = Cast<ATestPlayerState>(Player->PlayerState)->GetTeamId();;
		bool SameTeam = OtherTeamId == SelfTeamId;
		COUNT_STENCIL(OtherStencil, OtherTeamId, SameTeam, true);
		COUNT_STENCIL(SelfStencil, SelfTeamId, SameTeam, true);

		Cast<AWallRunnerCharacter>(OtherPlayer->GetCharacter())->ChangeStencil(
			Player->GetCharacter(), SelfStencil);
		Cast<AWallRunnerCharacter>(Player->GetCharacter())->ChangeStencil(
			OtherPlayer->GetCharacter(), OtherStencil);
	}
}

void ATestGameState::MulticastChangeScore_Implementation(ETeam ScoredTeamType, int NewScore)
{
	UE_LOG(LogTemp, Warning, TEXT("INCREASE SCORE CALLED for team %d"), ScoredTeamType);
	Cast<AWallRunnerCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter())->ChangeScoreUI(
		ScoredTeamType, NewScore);
	// TeamsByIndexes[ScoredTeamType]->IncreaseScore(NewScore);
}

void ATestGameState::ChangePlayerTeam_Implementation(APlayerController* Player)
{
	if (HasAuthority())
	{
		UTeamComponent* old_team = BlueTeam;
		UTeamComponent* new_team = RedTeam;
		if (Cast<ATestPlayerState>(Player->PlayerState)->Team == RedTeam)
		{
			Swap(old_team, new_team);
		}

		old_team->RemoveMember(Player);
		new_team->AddMember(Player);
		Cast<ATestPlayerState>(Player->PlayerState)->Team = new_team;

		ApplyPlayerStencil(Player);
		//Cast<AWallRunnerCharacter>(Player->GetCharacter())->ChangeFPSStencil(new_team->GetTeamId() + 12);
		UE_LOG(LogTemp, Warning, TEXT("Plyer with id %u is now in team %d"), Player->GetUniqueID(),
		       new_team->GetTeamType());
	}
}
