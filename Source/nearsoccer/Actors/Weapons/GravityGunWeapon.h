// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "Actors/Weapons/CombatWeapon.h"
#include "GravityGunWeapon.generated.h"


// TODO: Network grabbing, improve coefficients for grabbing
UCLASS(Blueprintable)
class NEARSOCCER_API AGravityGunWeapon : public ACombatWeapon
{
	GENERATED_BODY()

public:
	AGravityGunWeapon();

protected:
	// Setup on actor spawn
	virtual void BeginPlay() override;

public:
	// Ask gravity gun to poke object that is in LOS
	virtual void PrimaryFireBegin_Implementation() override;

	// Ask gravity gun to fully stop object that is in LOS
	virtual void SecondaryFireBegin_Implementation() override;

private:
	// Pointer to owners camera
	UPROPERTY()
	class UCameraComponent* Camera{nullptr};

	// Whether we can poke via gravity gun
	UPROPERTY(Replicated)
	bool bCanPoke{true};

	// Whether we can block with gravity gun
	UPROPERTY(Replicated)
	bool bCanBlock{true};

protected:
	// Max gravity gun search-for-object range
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Constants | Range")
	float GunSearchRange{450.0f};

	// Max gravity gun search-for-object while owner has gun buff activated range
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Constants | Range")
	float GunSearchBuffedRange{1600.0f};

	// Velocity that is used to launch soccer balls
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Constants | Gun Power")
	float LaunchBallCoefficient{4500.0f};
	
	// Velocity that is used to launch players
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Constants | Gun Power")
    float LaunchPlayerCoefficient{3000.0f};
	
	// Coefficient that is used when owner player has activated gun power buff
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Constants | Gun Power")
	float LaunchBuffedCoefficient{1.5f};

	// Cooldown between pokes
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Constants | Cooldown")
	float PokeCooldown{1.5f};

	// Cooldown between pokes
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Constants | Cooldown")
	float BlockCooldown{3.0f};

	// Poke cooldown handle
	UPROPERTY()
	FTimerHandle PokeCooldownHandle;

	// Block cooldown handle
	UPROPERTY()
	FTimerHandle BlockCooldownHandle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UNiagaraSystem* FXOnShot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UNiagaraSystem* FXOnPoke;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UNiagaraSystem* FXOnBlock;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundBase* SFXOnHit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundBase* SFXOnShot;

private:
	// Targets gravity gun can push
	enum class ETargetType {Target_Ball, Target_Player};

	// Get primitive component in line of sight that is at least as close as Range
	TOptional<TPair<ETargetType, UPrimitiveComponent*>> GetAimTarget(float Range, FHitResult& Hit) const;

	// Pokes object and sets cooldown
	UFUNCTION()
	void PokeObject(UPrimitiveComponent* Obj, FVector Impulse);

	// Pokes player and sets cooldown
	UFUNCTION()
	void PokePlayer(class AWallRunnerCharacter* Plr, FVector Impulse);

	// Allows poking
	UFUNCTION(Server, Reliable)
	void EnablePoking();

	// Blocks object
	UFUNCTION()
	void BlockObject(UPrimitiveComponent* Obj);

	// Allows blocking
	UFUNCTION(Server, Reliable)
	void EnableBlocking();

	const FLinearColor ColorFXDefault = {4.0f, 1.0f, 0.0f, 1.0f};
	const FLinearColor ColorFXBlock = {1.0f, 4.0f, 0.0f, 1.0f};
	
	// Hides / shows ready-to-fire indicator on all clients
	UFUNCTION(NetMulticast, Reliable)
	void SetIndicatorOwnerNoSee(class UNiagaraComponent* Indicator, bool bNewOwnerNoSee);

	// Spawns Niagara effects on each client
	UFUNCTION(NetMulticast, Reliable)
	void SpawnNiagaraEffects(UNiagaraSystem* Effect, FLinearColor Color, float Len = 0.0f);

	// Spawns sound effects on each client
	UFUNCTION(NetMulticast, Reliable)
	void SpawnSoundEffect(USoundBase* Effect);
};
