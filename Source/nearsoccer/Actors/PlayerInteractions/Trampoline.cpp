// Nearsoccer 2022


#include "Actors/PlayerInteractions/Trampoline.h"

// Sets default values
ATrampoline::ATrampoline()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComp;
}

// Called when the game starts or when spawned
void ATrampoline::BeginPlay()
{
	Super::BeginPlay();
	
}