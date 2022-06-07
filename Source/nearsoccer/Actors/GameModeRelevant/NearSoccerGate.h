// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "Core/NearsoccerCore.h"
#include "Engine/TriggerSphere.h"
#include "NearSoccerGate.generated.h"

/**
 * 
 */
UCLASS()
class NEARSOCCER_API ANearSoccerGate : public ATriggerSphere
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ANearSoccerGate();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="BP|Team")
	ETeam GateTeam;
	
	UPROPERTY(EditAnywhere, Category="BP|Radius")
	int GateRadius;

	UPROPERTY(EditAnywhere, Category="BP|Ball")
	TSubclassOf<AActor> BallToSpawn;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	ETeam GetTeam();

	int GetRadius();

	UFUNCTION()
	void SpawnBall();
};
