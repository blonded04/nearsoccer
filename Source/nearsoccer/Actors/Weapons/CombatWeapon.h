// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "CombatWeapon.generated.h"


// All the weapons used by player should be inherited from this class
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NEARSOCCER_API ACombatWeapon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ACombatWeapon();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void
	TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;

	// Handle start of primary fire
	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void PrimaryFireBegin();

	// Handle end of primary fire
	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void PrimaryFireEnd();

	// Handle start of secondary fire
	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void SecondaryFireBegin();

	// Handle end of secondary fire
	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void SecondaryFireEnd();

	// Handle reloading of the weapon
	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void Reload();

	// Disable fire of the weapon on death
	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void StopFire();
};
