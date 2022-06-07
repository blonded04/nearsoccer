// Nearsoccer 2022


#include "TeamComponent.h"
#include "Core/NearsoccerCore.h"
#include "TestPlayerState.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UTeamComponent::UTeamComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTeamComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UTeamComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UTeamComponent, PlayersCount);
	DOREPLIFETIME(UTeamComponent, TeamType);
	DOREPLIFETIME(UTeamComponent, Score);
	DOREPLIFETIME(UTeamComponent, PlayersArray);
	DOREPLIFETIME(UTeamComponent, TeamColor);
}


// Called every frame
void UTeamComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTeamComponent::Init(ETeam Type, FLinearColor TColor)
{
	if (IsInitialized)
		return;
	TeamType = Type;
	TeamColor = TColor;
	IsInitialized = true;
}

void UTeamComponent::AddMember(APlayerController* Player)
{
	VOID_IS_VALID(Player);
	PlayersArray.Push(Cast<ATestPlayerState>(Player->PlayerState));
	PlayersIndexes.Add(Player->GetUniqueID(), PlayersArray.Num() - 1);
	PlayersCount++;
}

void UTeamComponent::RemoveMember(APlayerController* Player)
{
	VOID_IS_VALID(Player);
	PlayersArray[PlayersIndexes[Player->GetUniqueID()]] = nullptr;
	PlayersIndexes.Remove(Player->GetUniqueID());
	PlayersCount--;
	UE_LOG(LogTemp, Warning, TEXT("Team %d has %d players"), TeamType, PlayersCount);
}

int UTeamComponent::GetScore() const
{
	return Score;
}

int UTeamComponent::GetPlayersCount() const
{
	UE_LOG(LogTemp, Warning, TEXT("Team %d has %d players"), TeamType, PlayersCount);
	return PlayersCount;
}

int UTeamComponent::GetTeamId() const
{
	return static_cast<int>(TeamType);
}

ETeam UTeamComponent::GetTeamType() const
{
	return TeamType;
}

FLinearColor UTeamComponent::GetColor() const
{
	return TeamColor;
}

void UTeamComponent::ChangeScore(int DeltaScore)
{
	Score += DeltaScore;
}

TArray<APlayerState*> UTeamComponent::GetPlayers()
{
	return PlayersArray;
}
