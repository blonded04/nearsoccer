// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NearsoccerPickup.generated.h"

class AWallRunnerCharacter;
class UStaticMeshComponent;

UCLASS()
class NEARSOCCER_API ANearsoccerPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANearsoccerPickup();

	// Adds PowerUp to inventory of player
	UFUNCTION(BlueprintCallable)
	virtual void PickUpActor(AWallRunnerCharacter* Player);

	void SetParent(AActor* Spawner);

	UFUNCTION(BlueprintCallable)
	AActor* GetParent();

protected:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, Category="Type")
	TSubclassOf<class UBaseBuff> BuffType;

	UPROPERTY()
	AActor* ParentSpawner;
	
	UPROPERTY(EditDefaultsOnly)
	USoundBase* PickupSound;

	// Destroys Actor
	UFUNCTION(NetMulticast, Reliable)
	void MultiDestroyActor();
	
	// Spawns Pickup Sound
	UFUNCTION(NetMulticast, Reliable)
	void MultiSpawnPickupSound(FVector SpawnLocation);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};