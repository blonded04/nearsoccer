// Nearsoccer 2022


#include "WallRunComponent.h"
#include "Core/NearsoccerCore.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "StaticMeshes/ForbiddenStaticMeshActor.h"

// Sets default values for this component's properties
UWallRunComponent::UWallRunComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UWallRunComponent::BeginPlay()
{
	Super::BeginPlay();

	// Initialize casted player owner
	Plr = GetOwner<ACharacter>();
	check(Plr != nullptr);
	if (Plr != nullptr && Plr->GetCharacterMovement() != nullptr)
	{
		Plr->GetCharacterMovement()->MaxFlySpeed = WallRidingSpeed;
	}
}

void UWallRunComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWallRunComponent, bIsRiding);
}

// Set mode flying on server for any client
void UWallRunComponent::SetFlying_Implementation(ACharacter* Player)
{
	VOID_IS_VALID(GetOwner());
	VOID_IS_VALID(Player);
	Player->GetCharacterMovement()->MovementMode = MOVE_Flying;
}

// Turn flying back off for any client
void UWallRunComponent::SetNotFlying_Implementation(
	ACharacter* Player)
{
	VOID_IS_VALID(GetOwner());
	VOID_IS_VALID(Player);
	Player->GetCharacterMovement()->MovementMode = MOVE_Walking;
}

// Allow player wall-riding (used for adding delay for reattaching)
void UWallRunComponent::AllowWallRiding()
{
	bCanWallRide = true;
}

// Lets wall riding player keep its speed on attach
void UWallRunComponent::UpdateFlyingSpeed_Implementation(
	ACharacter* Player, float Speed)
{
	if (Speed <= WallRidingSpeed + EPS)
	{
		Player->GetCharacterMovement()->MaxFlySpeed = WallRidingSpeed;
	}
	else
	{
		Player->GetCharacterMovement()->MaxFlySpeed = Speed;
	}
}

// Attaches player to the wall
void UWallRunComponent::AttachToWall(ACharacter* Player)
{
	VOID_IS_VALID(GetOwner());
	VOID_IS_VALID(Player);
	check(Player->GetCharacterMovement() != nullptr);
	check(bOnWall == false);

	UpdateFlyingSpeed(Player, Player->GetVelocity().Size());
	SetFlying(Player);
	Player->GetCharacterMovement()->Velocity.Z = std::max(Player->GetCharacterMovement()->Velocity.Z, 0.0f);

	// Slowly move towards the wall
	ToDecrease = VectorTowardsWallStraight.GetSafeNormal(0);
	if (bDrawDebugLines)
	{
		DrawDebugLine(GetWorld(), Player->GetActorLocation(),
		              Player->GetActorLocation() + WallFinderSensitivity * VectorAlongWall, FColor::Purple,
		              false,
		              10.0f);
		DrawDebugLine(GetWorld(), Player->GetActorLocation(),
		              Player->GetActorLocation() + VectorTowardsWallStraight, FColor::Red, false, 10.0f);
		DrawDebugLine(GetWorld(), Player->GetActorLocation(),
		              Player->GetActorLocation() + VectorTowardsWallRotated, FColor::Blue, false, 10.0f);
	}
	bOnWall = true;
	UpdateIsRiding(true);
	bWasRiding = true;
}

// Detaches player from the wall
void UWallRunComponent::DetachFromWall(ACharacter* Player)
{
	VOID_IS_VALID(GetOwner());
	VOID_IS_VALID(Player);
	bOnWall = false;
	UpdateIsRiding(false);
	SetNotFlying(Player);

	UpdateFlyingSpeed(Player, WallRidingSpeed);
	if (bDrawDebugLines)
	{
		DrawDebugLine(GetWorld(), Player->GetActorLocation(),
		              Player->GetActorLocation() + WallFinderSensitivity * VectorAlongWall, FColor::Purple,
		              false,
		              10.0f);
		DrawDebugLine(GetWorld(), Player->GetActorLocation(),
		              Player->GetActorLocation() + VectorTowardsWallStraight, FColor::Red, false, 10.0f);
		DrawDebugLine(GetWorld(), Player->GetActorLocation(),
		              Player->GetActorLocation() + VectorTowardsWallRotated, FColor::Blue, false, 10.0f);
	}

	// After successifully detaching boop owner
	BoopOwner();
}

// Need to receive angle between two vectors
float UWallRunComponent::CrossProd2D(const FVector& A, const FVector& B)
{
	return A.X * B.Y - A.Y * B.X;
}

// Ensure that hit hits a wall
bool UWallRunComponent::IsHitResOk(const FHitResult& Hit)
{
	if (Hit.Actor == nullptr)
	{
		return false;
	}
	AStaticMeshActor* HitRes = Cast<AStaticMeshActor>(Hit.Actor);
	if (HitRes == nullptr || HitRes->GetStaticMeshComponent()->Mobility == EComponentMobility::Movable || Cast<AForbiddenStaticMeshActor>(HitRes) != nullptr)
	{
		return false;
	}
	return true;
}

// Recalculate inner private member variables on successfull LookForWall
void UWallRunComponent::RecalcOnWallDetected(ACharacter* Player,
                                             const FVector& Location,
                                             const FVector& NewAlongWall,
                                             const FVector& StraightVectorCandidate,
                                             const FVector& RotatedVectorCandidate,
                                             const FVector& WallLocation)
{
	VOID_IS_VALID(GetOwner());
	VOID_IS_VALID(Player);

	// If we are already attached to wall we need to recalculate vector towards wall
	if (bOnWall)
	{
		check(NewAlongWall.SizeSquared() > EPS * EPS);

		float Angle = asin(std::max(-1.0f + EPS, std::min(1.0f - EPS,
		                                                  CrossProd2D(
			                                                  VectorAlongWall, NewAlongWall) /
		                                                  (VectorAlongWall.
			                                                  Size() * NewAlongWall.
			                                                  Size()))));
		VectorTowardsWallStraight = {
			static_cast<float>(VectorTowardsWallStraight.X * cos(Angle) -
				VectorTowardsWallStraight.Y * sin(Angle)),
			static_cast<float>(VectorTowardsWallStraight.X * sin(Angle) +
				VectorTowardsWallStraight.Y * cos(Angle)),
			0.0f
		};
		VectorTowardsWallRotated = {
			static_cast<float>(VectorTowardsWallRotated.X * cos(Angle) -
				VectorTowardsWallRotated.Y * sin(Angle)),
			static_cast<float>(VectorTowardsWallRotated.X * sin(Angle) +
				VectorTowardsWallRotated.Y * cos(Angle)),
			0.0f
		};
		VectorAlongWall = std::move(NewAlongWall).GetSafeNormal(0);

		// Recalc ToDecrease in case we are riding on a cillinder
		if (std::abs(Angle) >= EPS)
		{
			TeleportationVector = WallLocation - (Location +
				VectorTowardsWallStraight.GetSafeNormal(0) * Player->GetCapsuleComponent()->
				                                                     GetScaledCapsuleRadius());

			if (GEngine != nullptr && bDrawDebugMessages)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Purple, TEXT("Wall angle changed"));
			}
		}
	}
	else
	{
		VectorTowardsWallStraight = StraightVectorCandidate - Location;
		VectorTowardsWallStraight.Z = 0;
		VectorTowardsWallRotated = RotatedVectorCandidate - Location;
		VectorTowardsWallRotated.Z = 0;
		VectorAlongWall = std::move(NewAlongWall).GetSafeNormal(0);
	}
}


void UWallRunComponent::UpdateIsRiding_Implementation(bool Value)
{
	bIsRiding = Value;
}

// Work with attaching to the wall when wall ride has started
TOptional<FVector> UWallRunComponent::LookForWall(
	ACharacter* Player, const FVector& Location,
	FHitResult& Hit,
	const FCollisionQueryParams& TraceParams,
	const FVector& TraceEndLocation)
{
	VALUE_IS_VALID(GetOwner(), {});
	VALUE_IS_VALID(Player, {});
	GetWorld()->LineTraceSingleByChannel(Hit, Location, TraceEndLocation,
	                                     ECC_Visibility, TraceParams);

	// If we didn't hit anything we didn't attach to anything
	// Check if we hit static wall, not prop
	if (!IsHitResOk(Hit))
	{
		return {};
	}

	// To determine vector that we are going to move along the wall we will find one more trace-line hit result
	FVector ForwardMovementVector;
	if (bOnWall)
	{
		ForwardMovementVector = WallVectorFinderSensitivity * VectorAlongWall;
	}
	else
	{
		auto Vel = Player->GetVelocity();
		Vel.Z = 0;
		if (Vel.SizeSquared() > 100.0f)
		{
			Vel.Normalize();
			ForwardMovementVector = std::move(Vel);
		}
		else
		{
			ForwardMovementVector = Player->GetActorForwardVector();
		}
	}

	FVector NewAlongWallVector = FVector::VectorPlaneProject(ForwardMovementVector, Hit.Normal);
	NewAlongWallVector.Z = 0;
	if (NewAlongWallVector.SizeSquared() < EPS)
	{
		return {};
	}
	NewAlongWallVector.Normalize();

	// We have successfully hit the wall by now
	return NewAlongWallVector;
}

// Returng if we recalced vectors towards wall
bool UWallRunComponent::TryRecalc(ACharacter* Player, const FVector& Location,
                                  FHitResult& Hit, const FCollisionQueryParams& TraceParams,
                                  const FVector& TraceEndLocation, const FVector& StraightVectorCandidate,
                                  const FVector& RotatedVectorCandidate)
{
	TOptional<FVector> NewAlongWall = LookForWall(Player, Location, Hit, TraceParams, TraceEndLocation);
	if (NewAlongWall.IsSet())
	{
		NewAlongWall.GetValue();
		RecalcOnWallDetected(Player, Location, NewAlongWall.GetValue(), StraightVectorCandidate,
		                     RotatedVectorCandidate, Hit.Location);
		return true;
	}
	return false;
}

// Teleports player towards the cilinder
void UWallRunComponent::Teleport_Implementation(UWallRunComponent* Component)
{
	VOID_IS_VALID(this);
	VOID_IS_VALID(Component);
	VOID_IS_VALID(Component->Plr);

	if (Component->TeleportationVector.IsSet())
	{
		UE_LOG(LogTemp, Warning, TEXT("Angle changed, teleporting %p"), Component);
		
		Component->Plr->SetActorLocation(
		Component->Plr->GetActorLocation() + Component->TeleportationVector.GetValue(), true, nullptr,
			ETeleportType::TeleportPhysics);
		Component->TeleportationVector.Reset();
	}
}

// Called every frame
void UWallRunComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction*
                                      ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	VOID_IS_VALID(GetOwner())

	// If space is still pressed we keep searching for wall to ride
	bRideStarted = bTryRiding;

	if (bRideStarted && bCanWallRide)
	{
		// Determine Player's position in the world
		FVector Location = Plr->GetActorLocation();

		// Initialize Hit trace line result with default value
		FHitResult Hit;
		FCollisionQueryParams TraceParams;

		if (!bOnWall) // If we are not attached yet
		{
			VOID_IS_VALID(Plr)
			VOID_IS_VALID(Plr->Controller)

			// Right and left vectors to search in that directions
			auto Fwd = WallFinderSensitivity * Plr->GetActorForwardVector();
			auto RightDirection = WallFinderSensitivity * Plr->GetActorRightVector();
			auto LeftDirection = -RightDirection;

			// Now we need to rotate side vectors slightly forward, so that they
			// are sort of diagonal vectors
			const float RotCoef = 0.7;
			auto RightRot = RotCoef * RightDirection + (1 - RotCoef) * Fwd;
			RightRot.Z = 0;
			RightRot = RightRot.GetSafeNormal(0) * WallFinderSensitivity + Location;
			auto LeftRot = RotCoef * LeftDirection + (1 - RotCoef) * Fwd;
			LeftRot.Z = 0;
			LeftRot = LeftRot.GetSafeNormal(0) * WallFinderSensitivity + Location;

			// Right and left trace-lines to determine which wall to attach
			RightDirection = Location + RightDirection;
			LeftDirection = Location + LeftDirection;

			// Trace vectors to the closest objects to the right
			if (!TryRecalc(Plr, Location, Hit, TraceParams, RightDirection,
			               RightDirection, RightRot)
				&& !TryRecalc(Plr, Location, Hit, TraceParams, RightRot,
				              RightDirection, RightRot)
				&& !TryRecalc(Plr, Location, Hit, TraceParams, LeftDirection,
				              LeftDirection, LeftRot)
				&& !TryRecalc(Plr, Location, Hit, TraceParams, LeftRot,
				              LeftDirection, LeftRot))
			{
				// If we didn't hit anything we don't start riding
				bRideStarted = false;
			}

			// Attach to wall if we weren't attached to the wall before
			if (bRideStarted)
			{
				// We can double-jump after successfully attaching to wall
				Plr->JumpCurrentCount = 0;
				AttachToWall(Plr);
			}
		}
		else
		{
			// Use direction of current successful attaching to the wall
			auto VecStr = Location + VectorTowardsWallStraight;
			auto VecRot = Location + VectorTowardsWallRotated;

			// Trace vector to the closest objects to the direction towards wall
			if (!TryRecalc(Plr, Location, Hit, TraceParams, VecStr,
			               VecStr, VecRot)
				&& !TryRecalc(Plr, Location, Hit, TraceParams, VecRot,
				              VecStr, VecRot))
			{
				bRideStarted = false;
			}
		}
	}
	if (!bRideStarted && bOnWall) // Detach from wall if was riding before
	{
		DetachFromWall(Plr);
	}

	// If we're wall-riding we should move player in direction of wall-ride
	if (bRideStarted && bOnWall)
	{
		// Move player and decrease his distance to wall
		Plr->AddMovementInput(VectorAlongWall, WallRidingSpeed * DeltaTime);
		Plr->AddMovementInput(ToDecrease, WallFinderSensitivity * 100.0f);
		ToDecrease = FVector::ZeroVector;

		// In case if angle changed we should teleport player towards the angle changed wall
		Teleport(this);

		if (GEngine != nullptr && bDrawDebugMessages)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue,
			                                 FString::Printf(
				                                 TEXT("Speed on wall is: %f"),
				                                 Plr->GetCharacterMovement()
				                                    ->Velocity.Size()));
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue,
			                                 FString::Printf(
				                                 TEXT("VectorAlongWall is: %f %f %f"),
				                                 VectorAlongWall.X, VectorAlongWall.Y, VectorAlongWall.Z));
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue,
			                                 FString::Printf(
				                                 TEXT("ToDecrease is: %f %f %f"),
				                                 ToDecrease.X, ToDecrease.Y, ToDecrease.Z));
		}
	}
}

void UWallRunComponent::Boop_Implementation(ACharacter* Player, const FVector& Vec)
{
	auto LaunchVec = Vec.GetSafeNormal(0) / 1.3f + FRotationMatrix(Player->Controller->GetControlRotation()).
		GetScaledAxis(EAxis::X);
	LaunchVec.Normalize();
	Player->LaunchCharacter(LaunchVec * BoopCoefficient, false, false);
}


// Boop player in the direction he is looking to on detaching from wall
void UWallRunComponent::BoopOwner()
{
	VOID_IS_VALID(GetOwner())

	Boop(Plr, -VectorTowardsWallStraight);
}

// Handles beginning of riding when JUMP_KEY is pressed
void UWallRunComponent::BeginRiding()
{
	if (!Plr->GetCharacterMovement()->IsCrouching())
	{
		bTryRiding = true;
	}
}

// Handles stop of riding when JUMP_KEY is released
void UWallRunComponent::StopRiding()
{
	VOID_IS_VALID(GetOwner())
	bTryRiding = false;
	if (bWasRiding)
	{
		bCanWallRide = false;

		Plr->GetWorldTimerManager().SetTimer(AttachTimerHandle, this,
		                                     &UWallRunComponent::AllowWallRiding,
		                                     AttachDelay);
	}

	bWasRiding = false;
}

bool UWallRunComponent::IsOnWall()
{
	return bOnWall;
}

bool UWallRunComponent::IsRiding()
{
	return bIsRiding;
}
