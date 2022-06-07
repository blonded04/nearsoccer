// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WallRunComponent.generated.h"

// TODO: Tweak cillinder-riding --- maybe change GetCharacterMovement()->Velocity to move on a plane

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NEARSOCCER_API UWallRunComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWallRunComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDrawDebugMessages{false};

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDrawDebugLines{false};

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction*
	                           ThisTickFunction) override;

	// Handles beginning of riding when JUMP_KEY is pressed
	UFUNCTION(BlueprintCallable)
	void BeginRiding();

	// Handles stop of riding when JUMP_KEY is released
	UFUNCTION(BlueprintCallable)
	void StopRiding();

	// Allow player wall-riding (used for adding delay for reattaching)
	UFUNCTION(BlueprintCallable)
	void AllowWallRiding();

private:
	// Boop player in the direction he is looking to on detaching from wall
	UFUNCTION(BlueprintCallable)
	void BoopOwner();

public:
	UFUNCTION(BlueprintCallable)
	bool IsOnWall();
	
	UFUNCTION(BlueprintCallable)
	bool IsRiding();

	// Is wall-ride cooldown expired / can we ever wall-ride
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall-riding")
	bool bCanWallRide{true};

	// Speed while wall-riding
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall-riding | Speed")
	float WallRidingSpeed{800.0f};

	// Speed at which owner is launched after detaching
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall-riding | Speed")
	float BoopCoefficient{325.0f};

private:
	// Attaches player to the wall
	UFUNCTION(BlueprintCallable)
	void AttachToWall(ACharacter* Player);

	// Detaches player from the wall
	UFUNCTION(BlueprintCallable)
	void DetachFromWall(ACharacter* Player);

	// Set mode flying on server for any client
	UFUNCTION(Server, Reliable)
	void SetFlying(ACharacter* Player);

	// Turn flying back off for any client
	UFUNCTION(Server, Reliable)
	void SetNotFlying(ACharacter* Player);

	// Timer for player to safely reattach after detaching to the same wall
	// to boop of the wall properly
	FTimerHandle AttachTimerHandle;

	// Ensure that hit hits a wall
	static bool IsHitResOk(const FHitResult& Hit);

	// Need to receive angle between two vectors
	static float CrossProd2D(const FVector& A, const FVector& B);

	// Direction owner is moving to if attached to wall
	FVector VectorAlongWall;

	// Direction to search for wall at (to prevent buggy wall rides)
	// * Determined after successfully attaching to wall
	// * Not normalized
	FVector VectorTowardsWallStraight;

	// Secondary direction to search for wall at
	FVector VectorTowardsWallRotated;

	// Keep trying wall-riding while JUMP_KEY is pressed
	bool bTryRiding{false};

	// Store if wall riding is ongoing
	bool bRideStarted{false};

	// Check if owner is attached to wall to safely detach him when not riding
	bool bOnWall{false};

	UPROPERTY(Replicated)
	bool bIsRiding{false};
	
	UFUNCTION(Server, Reliable)
	void UpdateIsRiding(bool Value);

	// Check if owner was riding before detaching
	bool bWasRiding{false};

	// Work with attaching to the wall when wall ride has started
	TOptional<FVector> LookForWall(ACharacter* Player,
	                               const FVector& Location,
	                               FHitResult& Hit,
	                               const FCollisionQueryParams& TraceParams,
	                               const FVector& TraceEndLocation);

	// Recalculate inner private member variables on successfull LookForWall
	void RecalcOnWallDetected(ACharacter* Player, const FVector& Location,
							  const FVector& NewAlongWall, const FVector& StraightVectorCandidate,
							  const FVector& RotatedVectorCandidate, const FVector& WallLocation);

	// Returng if we recalced vectors towards wall
	bool TryRecalc(ACharacter* Player,
	               const FVector& Location,
	               FHitResult& Hit,
	               const FCollisionQueryParams& TraceParams,
	               const FVector& TraceEndLocation,
	               const FVector& StraightVectorCandidate,
	               const FVector& RotatedVectorCandidate);


	// Lets wall riding player keep its speed on attach
	UFUNCTION(Server, Reliable)
	void UpdateFlyingSpeed(ACharacter* Player, float Speed);

	// Boops player on server
	UFUNCTION(Server, Reliable)
	void Boop(ACharacter* Player, const FVector& Vec);

	// Teleports player towards the cilinder
	UFUNCTION(NetMulticast, Server, Reliable)
	void Teleport(UWallRunComponent *Component);

	// Wall-ride wall-finder vector size units
	float WallFinderSensitivity{200.0f};

	// Sensitivity to determine which way should character move along the wall
	float WallVectorFinderSensitivity{3.0f};

	// Delay for timer that handles reattaching after booping of the wall
	float AttachDelay{0.5f};

	// Precision for floating point numbers comparison
	const float EPS{0.001f};

	// Increased pull towards the wall rate
	float PullCoefficient{10.5f};

	// Slowly decrease Z velocity and distance to wall when attached to the wall
	FVector ToDecrease{0.0f};

	// Teleport towards the wall for cillinder-riding
	TOptional<FVector> TeleportationVector;

	// Pointer to owner
	UPROPERTY()
	ACharacter* Plr;
};