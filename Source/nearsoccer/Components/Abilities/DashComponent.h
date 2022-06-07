// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DashComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NEARSOCCER_API UDashComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDashComponent();

	// Allow our character to dash
	UPROPERTY(EditAnywhere)
	bool bCanDash{true};

	// Dash distance
	UPROPERTY(EditAnywhere)
	float DashDistance{10000.0f};

	// Dash sound
	UPROPERTY(EditAnywhere)
	USoundBase* DashSound;

	// Dash cooldown
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	float DashCooldown{5.0f};

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// Dash cooldown handle
	UPROPERTY(Replicated)
	FTimerHandle DashHandle;

	// Postdash handle
	UPROPERTY(Replicated)
	FTimerHandle PostDashHandle;

	// Allow dashing after cooldown is over
	void AllowDashing();

	// Ground Friction before dash
	UPROPERTY(Replicated)
	float PendingFriction{0.0f};

	UPROPERTY()
	ACharacter* Player;

	// Needed for passing info to HUD
	UPROPERTY(Replicated)
	bool bDashedRecently{false};

	// Perform dash
	UFUNCTION(Server, Reliable)
	void Dash(UDashComponent* Component);
	
	// Reset ground friction and stop immediately after dash
	UFUNCTION(Server, Reliable)
	void PostDash();

public:
	// Needed for passing info to HUD
	UFUNCTION(BlueprintCallable)
	bool isDashedRecently() { return bDashedRecently; }

	// Process dash input
	UFUNCTION()
	void DashInput();

	// Resets bDashedRecently
	UFUNCTION(NetMulticast, Reliable)
	void ServerDashProcessed(UDashComponent* Component, const bool Val);
	
	// Spawns dash sound
	UFUNCTION(NetMulticast, Reliable)
	void MultiSpawnSound(ACharacter* AttachPlayer);
};
