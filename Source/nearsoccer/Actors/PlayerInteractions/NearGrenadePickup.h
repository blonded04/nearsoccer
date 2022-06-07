// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "Actors/PlayerInteractions/NearsoccerPickup.h"
#include "GameFramework/Actor.h"
#include "NearGrenadePickup.generated.h"

class ANearGrenade;

UCLASS()
class NEARSOCCER_API ANearGrenadePickup : public ANearsoccerPickup
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category="Type")
	TSubclassOf<ANearGrenade> GrenadeType;

public:
	virtual void PickUpActor(AWallRunnerCharacter* Player) override;
};