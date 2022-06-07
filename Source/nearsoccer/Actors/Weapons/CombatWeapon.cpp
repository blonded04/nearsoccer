// Nearsoccer 2022


#include "Actors/Weapons/CombatWeapon.h"

// Sets default values for this component's properties
ACombatWeapon::ACombatWeapon()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts
void ACombatWeapon::BeginPlay()
{
	Super::BeginPlay();

	SetReplicates(true);
}

// Called every frame
void ACombatWeapon::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);
}

// Handle start of primary fire
void ACombatWeapon::PrimaryFireBegin_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("CombatWeapon: Combat Weapon was asked to primary fire"));
}

// Handle end of primary fire
void ACombatWeapon::PrimaryFireEnd_Implementation()
{
}

// Handle start of secondary fire
void ACombatWeapon::SecondaryFireBegin_Implementation()
{
}

// Handle end of secondary fire
void ACombatWeapon::SecondaryFireEnd_Implementation()
{
}

// Handle reloading of the weapon
void ACombatWeapon::Reload_Implementation()
{
}

void ACombatWeapon::StopFire_Implementation()
{
	PrimaryFireEnd();
	SecondaryFireEnd();
}
