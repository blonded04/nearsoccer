// Nearsoccer 2022


#include "Actors/PlayerInteractions/NearGrenadePickup.h"
#include "WallRunnerCharacter.h"



void ANearGrenadePickup::PickUpActor(AWallRunnerCharacter* Player)
{
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Setting Grenade Type"));
		Player->InventoryComp->SetGrenadeType(GrenadeType);
	}
	Super::PickUpActor(Player);
}
