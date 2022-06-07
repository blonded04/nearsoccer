// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "NearsoccerPickup.h"
#include "GameFramework/Actor.h"
#include "PickupSpawner.generated.h"

UCLASS()
class NEARSOCCER_API APickupSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SpawnPickup();
	
	UPROPERTY(EditDefaultsOnly, Category="Respawn | Timer")
	float RespawnTimer{5.0f};

	UPROPERTY(EditDefaultsOnly, Category="Respawn | Types")
	TArray<TSubclassOf<ANearsoccerPickup>> ActorsToSpawn;
	
	FTimerHandle RespawnHandle;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void PrepareToSpawn();

};
