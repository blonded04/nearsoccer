// Nearsoccer 2022


#include "Components/Abilities/InventoryComponent.h"
#include "Actors/ObjectsWithPhysics/NearGrenade.h"
#include "BaseBuff.h"
#include "BuffsFunctionLib.h"
#include "TestPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "Core/NearsoccerCore.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WallRunnerCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	PowerUpType = nullptr;
	GrenadeType = nullptr;
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// Replicate to everyone
	DOREPLIFETIME(UInventoryComponent, PowerUpType);
	DOREPLIFETIME(UInventoryComponent, GrenadeType);
}

//============================POWERUPS==============================

FBuffInfo UInventoryComponent::GetPowerUpInfo()
{
	FBuffInfo DefaultReturn;
	VALUE_IS_VALID(PowerUpType, DefaultReturn);

	return FBuffInfo(Cast<UBaseBuff>(PowerUpType->GetDefaultObject()));
}


void UInventoryComponent::SetPowerUpType(TSubclassOf<UBaseBuff> Value)
{
	VOID_IS_VALID(GetOwner());

	if (!GetOwner()->HasAuthority())
	{
		ServerSetPowerUpType(Value);
	}
	else
	{
		PowerUpType = Value;
	}
}

void UInventoryComponent::ServerSetPowerUpType_Implementation(TSubclassOf<UBaseBuff> Value)
{
	VOID_IS_VALID(GetOwner());
	if (GetOwner()->HasAuthority())
	{
		SetPowerUpType(Value);
	}
}

void UInventoryComponent::ActivatePowerUp()
{
	VOID_IS_VALID(GetOwner());
	ACharacter* Player = Cast<ACharacter>(GetOwner());
	if (!GetOwner()->HasAuthority())
	{
		ServerActivatePowerUp();
	}
	else
	{
		if (PowerUpType != nullptr)
		{
			ATestPlayerState* PlayerState = Cast<ATestPlayerState>(Player->GetPlayerState());
			ATestGameMode* GameMode = Cast<ATestGameMode>(Player->GetWorld()->GetAuthGameMode());
			UE_LOG(LogTemp, Warning, TEXT("UBuffsFunctionLib::ApplyBuffAtLocation"));
			UBuffsFunctionLib::ApplyBuffAtLocation(PowerUpType, Player->GetActorLocation(),
			                                       PlayerState->GetTeamType(), true, GameMode,
			                                       PowerUpSound);
			PowerUpType = nullptr;
		}
	}
}

void UInventoryComponent::ServerActivatePowerUp_Implementation()
{
	VOID_IS_VALID(GetOwner());
	if (GetOwner()->HasAuthority())
	{
		ActivatePowerUp();
	}
}

//============================GRENADES==============================


FBuffInfo UInventoryComponent::GetGrenadeInfo()
{
	FBuffInfo DefaultReturn;
	VALUE_IS_VALID(GrenadeType, DefaultReturn);

	ANearGrenade* Grenade = Cast<ANearGrenade>(GrenadeType->GetDefaultObject());
	return Grenade->GetGrenadeInfo();
}

void UInventoryComponent::SetGrenadeType(TSubclassOf<ANearGrenade> Value)
{
	VOID_IS_VALID(GetOwner());

	if (!GetOwner()->HasAuthority())
	{
		ServerSetGrenadeType(Value);
	}
	else
	{
		GrenadeType = Value;
	}
}

void UInventoryComponent::ServerSetGrenadeType_Implementation(TSubclassOf<ANearGrenade> Value)
{
	VOID_IS_VALID(GetOwner());
	if (GetOwner()->HasAuthority())
	{
		SetGrenadeType(Value);
	}
}

void UInventoryComponent::ThrowGrenade(FVector Location, FRotator Rotation)
{
	VOID_IS_VALID(GetOwner());
	if (!GetOwner()->HasAuthority())
	{
		ServerThrowGrenade(Location, Rotation);
	}
	else
	{
		ACharacter* Player = Cast<ACharacter>(GetOwner());
		if (!Player)
		{
			return;
		}
		ATestPlayerState* PlayerState = Cast<ATestPlayerState>(Player->GetPlayerState());
		if (GrenadeType != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Throwing"));
			MultiSpawnThrowSound(Player);
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			ANearGrenade* Grenade = GetWorld()->SpawnActor<ANearGrenade>(
				GrenadeType, Location, Rotation, ActorSpawnParams);
			if (Grenade != nullptr)
			{
				Grenade->SetGameMode(Player->GetWorld()->GetAuthGameMode());
				Grenade->SetTeam(PlayerState->GetTeamType());
				Grenade->SetGrenadeOwner(PlayerState);
				if (AWallRunnerCharacter* WR_Character = Cast<AWallRunnerCharacter>(Player))
				{
					FVector Impulse = WR_Character->GetVelocity() * Grenade->MeshComp->GetMass();
					UE_LOG(LogTemp, Warning, TEXT("Impulse: %f, %f, %f"), Impulse.X, Impulse.Y, Impulse.Z);
					Grenade->MeshComp->AddImpulse(Impulse);
					Grenade->SetTrailColor(WR_Character->GetPlayerColor());
				}
				GrenadeType = nullptr;
			}
		}
	}
}

void UInventoryComponent::ServerThrowGrenade_Implementation(FVector Location, FRotator Rotation)
{
	VOID_IS_VALID(GetOwner());
	if (GetOwner()->HasAuthority())
	{
		ThrowGrenade(Location, Rotation);
	}
}

void UInventoryComponent::MultiSpawnThrowSound_Implementation(ACharacter* Character)
{
	if (ThrowSound)
	{
		UGameplayStatics::SpawnSoundAttached(ThrowSound, Character->GetMesh());
	}
}
