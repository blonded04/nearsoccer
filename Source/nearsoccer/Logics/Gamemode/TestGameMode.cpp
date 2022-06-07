// Nearsoccer 2022


#include "TestGameMode.h"
#include "TestGameInstance.h"
#include "TestPlayerState.h"
#include "TestGameState.h"
#include "WallRunnerCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"
#include "Core/NearsoccerCore.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Public/EngineUtils.h"
#include "Runtime/Core/Public/Math/UnrealMathUtility.h"

ATestGameMode::ATestGameMode() : Super()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(
		TEXT("/Game/Blueprints/Character/BP_WallRunnerCharacter"));
	if (PlayerPawnClassFinder.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnClassFinder.Class;

		// set player state
		PlayerStateClass = ATestPlayerState::StaticClass();
	}
}

void ATestGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("GameMode BeginPlay"));
}

FString ATestGameMode::InitNewPlayer(APlayerController* NewPlayerController,
                                     const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	FString ToReturn = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
	Cast<ATestGameState>(GameState)->AssignPlayerToTeam(NewPlayerController);
	return ToReturn;
}

void ATestGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	VOID_IS_VALID(NewPlayer);
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	Cast<ATestGameState>(GameState)->SetPlayerName(NewPlayer);
}

AActor* ATestGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	// Choose a teamed player start
	ATeamPlayerStart* FoundPlayerStart = nullptr;
	ETeam PlayerTeam = Player->GetPlayerState<ATestPlayerState>()->GetTeamType();
	UClass* PawnClass = GetDefaultPawnClassForController(Player);
	APawn* PawnToFit = PawnClass ? PawnClass->GetDefaultObject<APawn>() : nullptr;
	TArray<ATeamPlayerStart*> UnOccupiedStartPoints;
	TArray<ATeamPlayerStart*> OccupiedStartPoints;
	UWorld* World = GetWorld();
	auto StartType = GetGameState<ATestGameState>()->GetCurrentStartType();
	for (TActorIterator<ATeamPlayerStart> It(World); It; ++It)
	{
		ATeamPlayerStart* PlayerStart = *It;
		if (PlayerStart->GetType() != StartType)
		{
			continue;
		}
		if (PlayerStart->GetTeam() == PlayerTeam)
		{
			FVector ActorLocation = PlayerStart->GetActorLocation();
			const FRotator ActorRotation = PlayerStart->GetActorRotation();
			if (!World->EncroachingBlockingGeometry(PawnToFit, ActorLocation, ActorRotation))
			{
				UnOccupiedStartPoints.Add(PlayerStart);
			}
			else if (World->FindTeleportSpot(PawnToFit, ActorLocation, ActorRotation))
			{
				OccupiedStartPoints.Add(PlayerStart);
			}
		}
	}
	if (FoundPlayerStart == nullptr)
	{
		if (UnOccupiedStartPoints.Num() > 0)
		{
			FoundPlayerStart = UnOccupiedStartPoints[FMath::RandRange(0, UnOccupiedStartPoints.Num() - 1)];
		}
		else if (OccupiedStartPoints.Num() > 0)
		{
			FoundPlayerStart = OccupiedStartPoints[FMath::RandRange(0, OccupiedStartPoints.Num() - 1)];
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Choosed Player Start"));
	return FoundPlayerStart;
}

void ATestGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	AllPlayersInGame.Add(NewPlayer);
	ATestPlayerState* PState = Cast<ATestPlayerState>(NewPlayer->PlayerState);
	auto Character = Cast<AWallRunnerCharacter>(NewPlayer->GetCharacter());
	Character->SetTeamId(PState->GetTeamId());
	Cast<ATestGameState>(GameState)->ApplyPlayerStencil(NewPlayer);
}

void ATestGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	auto GS = Cast<ATestGameState>(GameState);
	ATestPlayerState* PState = Cast<ATestPlayerState>(Exiting->PlayerState);
	PState->Team->RemoveMember(Cast<APlayerController>(Exiting));
}


void ATestGameMode::RestartPlayer(AController* NewPlayer)
{
	NewPlayer->Reset();
	Super::RestartPlayer(NewPlayer);

	auto Character = Cast<AWallRunnerCharacter>(NewPlayer->GetCharacter());
	Cast<AWallRunnerCharacter>(NewPlayer->GetCharacter())->DrawHUDWidgets();
	ATestPlayerState* PState = NewPlayer->GetPlayerState<ATestPlayerState>();
	Character->SetTeamId(PState->GetTeamId());
	auto TestGameState = Cast<ATestGameState>(GameState);
	TestGameState->ApplyPlayerStencil(Cast<APlayerController>(NewPlayer));
	TestGameState->RebuildInfoWidgets(Cast<AWallRunnerCharacter>(NewPlayer->GetCharacter()));
}

void ATestGameMode::Respawn(AController* Controller)
{
	if (GetLocalRole() == ROLE_Authority && Controller)
	{
		FTimerDelegate RespawnDele;
		FTimerHandle RespawnHandle;
		RespawnDele.BindUFunction(this, FName("RestartPlayer"), Controller);
		GetWorld()->GetTimerManager().SetTimer(RespawnHandle, RespawnDele,
		                                       RespawnTimer, false);
	}
}

float ATestGameMode::GetRespawnTimer()
{
	return RespawnTimer;
}

TArray<APlayerController*> ATestGameMode::GetAllPlayers()
{
	return AllPlayersInGame;
}
