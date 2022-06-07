// Nearsoccer 2022


#include "Actors/GameModeRelevant/NearSoccerGate.h"

ANearSoccerGate::ANearSoccerGate()
{
	GateTeam = ETeam::ENone;

	GateRadius = 400;
}

void ANearSoccerGate::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority()) 
	{
	    SpawnBall();
	}
}

void ANearSoccerGate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

ETeam ANearSoccerGate::GetTeam() 
{
    return GateTeam;
}

int ANearSoccerGate::GetRadius()
{
	return GateRadius;
}

void ANearSoccerGate::SpawnBall()
{
	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotation = GetActorRotation();
	FActorSpawnParameters SpawnParams;
	AActor* SpawnedBallRef = GetWorld()->SpawnActor<AActor>(BallToSpawn, SpawnLocation, SpawnRotation, SpawnParams);
}
