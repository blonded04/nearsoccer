// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "Actors/GameModeRelevant/NearSoccerGate.h"
#include "Character/WallRunnerController.h"
#include "Core/NearsoccerCore.h"
#include "Engine/StaticMeshActor.h"
#include "NearsoccerSoccerBall.generated.h"

UCLASS()
class NEARSOCCER_API ANearsoccerSoccerBall : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANearsoccerSoccerBall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="BP|Team")
	ETeam BallTeam;
	
	UPROPERTY(EditAnywhere, Category="BP|Tick")
	float DeltaSeconds;
	
	UPROPERTY(EditAnywhere, Category="BP|Points")
	int BallPoints;
	
	UPROPERTY(EditAnywhere, Category="BP|Points")
	int BallPointsDecreaseValue;
	
	UPROPERTY(EditAnywhere, Category="BP|Niagara")
	class UNiagaraSystem* DestroyEffect;
	
	UPROPERTY(EditAnywhere, Category="BP|Niagara")
	FLinearColor ParticleColor;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SFX")
	USoundBase* HitSFX{};
	
	TArray<ANearSoccerGate*> EnemyGates;

	ANearSoccerGate* SpawnGate;
	
	FTimerHandle TickHandle;

	UPROPERTY(Replicated)
	AWallRunnerController *LastPlayerPoked{nullptr};

	UPROPERTY(Replicated)
	AWallRunnerController *LastPlayerBlocked{nullptr};

public:
	ETeam GetTeam();
	
	UFUNCTION(Server, Reliable)
	void SetLastPlayerPoked(AWallRunnerController *Val);

	UFUNCTION(BlueprintCallable)
	AWallRunnerController *GetLastPlayerPoked() const;
	
	UFUNCTION(Server, Reliable)
	void SetLastPlayerBlocked(AWallRunnerController *Val);

	UFUNCTION(BlueprintCallable)
	AWallRunnerController *GetLastPlayerBlocked() const;
	
protected:
	bool IsGateNear(ANearSoccerGate *GateActor);

	void TransferPoints(ETeam ToTeam);

	void CallDestroy();

	void BallTick();
	
	void SpawnBurst();

	UFUNCTION(Server, Reliable)
	void ServerCallSpawnBurst();

	UFUNCTION(NetMulticast, Reliable)
	void MultiSpawnBurst();
};
