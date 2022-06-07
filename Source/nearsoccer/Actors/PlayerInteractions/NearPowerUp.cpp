// Nearsoccer 2022


#include "NearPowerUp.h"
#include "WallRunnerCharacter.h"

void ANearPowerUp::PickUpActor(AWallRunnerCharacter* Player)
{
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Setting PowerUp Type"));
		Player->InventoryComp->SetPowerUpType(BuffType);
	}
	Super::PickUpActor(Player);
}