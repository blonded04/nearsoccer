// Nearsoccer 2022


#include "Components/Abilities/DashComponent.h"

#include "Net/UnrealNetwork.h"
#include "NearsoccerCore.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UDashComponent::UDashComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UDashComponent::BeginPlay()
{
	Super::BeginPlay();

	SetIsReplicated(true);
	
	Player = GetOwner<ACharacter>();
	check(Player != nullptr);
}

void UDashComponent::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// Replicate to everyone
	DOREPLIFETIME(UDashComponent, DashCooldown);
	DOREPLIFETIME(UDashComponent, DashHandle);
	DOREPLIFETIME(UDashComponent, PostDashHandle);
	DOREPLIFETIME(UDashComponent, PendingFriction);
	DOREPLIFETIME(UDashComponent, bDashedRecently);
}

// Allow dashing after cooldown is over
void UDashComponent::AllowDashing()
{
	bCanDash = true;
}

// Perform dash
void UDashComponent::Dash_Implementation(UDashComponent *Component)
{
	VOID_IS_VALID(Component);

	if (Component->bCanDash)
	{
		Component->bCanDash = false;
		Component->ServerDashProcessed(Component, true);
		Component->PendingFriction = Component->Player->GetCharacterMovement()->GroundFriction;
		Component->Player->GetCharacterMovement()->GroundFriction = 0.0f;
		MultiSpawnSound(Component->Player);
		Component->Player->LaunchCharacter(
			FRotationMatrix(Component->Player->Controller->GetControlRotation()).
			GetScaledAxis(EAxis::X) * Component->DashDistance, true, true);
		Component->GetWorld()->GetTimerManager().SetTimer(Component->PostDashHandle, Component,
        		                                        &UDashComponent::PostDash, 0.03);
		Component->GetWorld()->GetTimerManager().SetTimer(Component->DashHandle, Component,
		                                        &UDashComponent::AllowDashing, Component->DashCooldown);
	}
}

// Reset ground friction and stop immediately after dash
void UDashComponent::PostDash_Implementation()
{
	Player->GetCharacterMovement()->GroundFriction = PendingFriction;
	Player->GetCharacterMovement()->StopMovementImmediately();
	
	//Player->GetCharacterMovement()->Velocity /= Player->GetCharacterMovement()->Velocity.Size();
	//Player->GetCharacterMovement()->Velocity *= Player->GetCharacterMovement()->MaxWalkSpeed;
	//Player->GetCharacterMovement()->Velocity.Z = std::min(100.0f, Player->GetCharacterMovement()->Velocity.Z);
	
}

// Process dash input
void UDashComponent::DashInput()
{
	Dash(this);
}

void UDashComponent::MultiSpawnSound_Implementation(ACharacter* AttachPlayer)
{
	if (DashSound)
	{
		UGameplayStatics::SpawnSoundAttached(DashSound, AttachPlayer->GetMesh());
	}
}

// Resets bDashedRecently
void UDashComponent::ServerDashProcessed_Implementation(UDashComponent* Component, const bool Val)
{
	Component->bDashedRecently = Val;
}
