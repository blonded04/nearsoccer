// Nearsoccer 2022


#include "Actors/GameModeRelevant/PickupSpawner.h"

// Sets default values
APickupSpawner::APickupSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APickupSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SpawnPickup();
	}
}

// Called every frame
void APickupSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickupSpawner::PrepareToSpawn()
{
	GetWorld()->GetTimerManager().ClearTimer(RespawnHandle);
	GetWorld()->GetTimerManager().SetTimer(RespawnHandle, this,
		&APickupSpawner::SpawnPickup, RespawnTimer, false);
}

void APickupSpawner::SpawnPickup()
{
	TSubclassOf<ANearsoccerPickup> ActorType = ActorsToSpawn[FMath::RandRange(0, ActorsToSpawn.Num() - 1)];
	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotation = GetActorRotation();
	FActorSpawnParameters SpawnParams;
	AActor* SpawnedActorRef = GetWorld()->SpawnActor<AActor>(ActorType, SpawnLocation, SpawnRotation, SpawnParams);
	ANearsoccerPickup* ChildPU = Cast<ANearsoccerPickup>(SpawnedActorRef);
	if (ChildPU != nullptr)
	{
		ChildPU->SetParent(this);
	} else
	{
		PrepareToSpawn();
	}
}
