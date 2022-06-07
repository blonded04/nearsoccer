// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "Actors/PlayerInteractions/NearsoccerPickup.h"
#include "GameFramework/Actor.h"
#include "NearPowerUp.generated.h"

class AWallRunnerCharacter;
class UStaticMeshComponent;

UCLASS()
class NEARSOCCER_API ANearPowerUp : public ANearsoccerPickup
{
	GENERATED_BODY()

public:
	virtual void PickUpActor(AWallRunnerCharacter* Player) override;
};