// Nearsoccer 2022


#include "Actors/PlayerInteractions/NearsoccerPickup.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "WallRunnerCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ANearsoccerPickup::ANearsoccerPickup()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	RootComponent = MeshComp;
	ParentSpawner = nullptr;
}

void ANearsoccerPickup::MultiSpawnPickupSound_Implementation(FVector SpawnLocation)
{
	if (PickupSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), PickupSound, SpawnLocation);
	}
}

// Called when the game starts or when spawned
void ANearsoccerPickup::BeginPlay()
{
	Super::BeginPlay();
}

void ANearsoccerPickup::MultiDestroyActor_Implementation()
{
	Destroy();
}

void ANearsoccerPickup::PickUpActor(AWallRunnerCharacter* Player)
{
	if (HasAuthority())
	{
		MultiSpawnPickupSound(Player->GetMesh()->GetComponentLocation());
		MultiDestroyActor();
	}
}

void ANearsoccerPickup::SetParent(AActor* Spawner)
{
	ParentSpawner = Spawner;
}

AActor* ANearsoccerPickup::GetParent()
{
	return ParentSpawner;
}
