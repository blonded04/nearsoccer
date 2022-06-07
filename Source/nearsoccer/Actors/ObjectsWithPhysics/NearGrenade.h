// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "BuffInfo.h"
#include "NiagaraCommon.h"
#include "TestPlayerState.h"
#include "GameFramework/Actor.h"
#include "Core/NearsoccerCore.h"
#include "NearGrenade.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class NEARSOCCER_API ANearGrenade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANearGrenade();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComp;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle FuseHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UNiagaraComponent* TrailComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	UNiagaraSystem* BurstEffect;

	/** Projectile movement component */
	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, Category="Variables")
	float FuseTime{3.0f};

	UPROPERTY(EditDefaultsOnly, Category="Variables")
	float ExplosionRadius{300.0f};

	UPROPERTY(EditAnywhere, Category="Variables")
	TSubclassOf<class UBaseBuff> BuffType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Variables")
	bool bOnAllPlayers;
	
	UPROPERTY(EditAnywhere, Category="Variables")
	bool bWhenVisible{false};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Variables")
	USoundBase* ExplosionSFX{};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Variables")
	USoundBase* HitSFX{};

	UPROPERTY(BlueprintReadOnly, Category="Variables")
	ETeam InstigatorTeam {ETeam::ENone};

	AGameModeBase* GameMode;
	
	UPROPERTY(ReplicatedUsing=OnRep_TrailColor)
	FLinearColor TrailColor;

	UFUNCTION()
	void OnRep_TrailColor();

	UPROPERTY()
	ATestPlayerState* GrenadeOwner;

public:
	UFUNCTION(BlueprintCallable)
	FBuffInfo GetGrenadeInfo() const;

	void SetGameMode(AGameModeBase* GM);

	void SetTeam(ETeam PlayerTeam);
	
	void SetGrenadeOwner(ATestPlayerState* OwnerState);
	
	void SetTrailColor(FLinearColor PlayerColor);

	TSubclassOf<UBaseBuff> GetBuffType() const { return BuffType; }
	
protected:
	UFUNCTION(BlueprintCallable)
	void Explode();

	void CallDestroy();

	UFUNCTION(NetMulticast, Reliable)
	void MultiSpawnBurst();
	
	UFUNCTION(Server, Reliable)
	void ServerSpawnBurst();

	UFUNCTION(Server, Reliable)
	void ServerSetTrailColor(FLinearColor PlayerColor);
	
	UFUNCTION(BlueprintNativeEvent)
	void SpawnExplosionSFX();
};
