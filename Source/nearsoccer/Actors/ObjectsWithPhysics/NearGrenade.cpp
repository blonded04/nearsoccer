// Nearsoccer 2022


#include "Actors/ObjectsWithPhysics/NearGrenade.h"
#include "BuffsFunctionLib.h"
#include "BaseBuff.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ANearGrenade::ANearGrenade()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	RootComponent = MeshComp;

	TrailComp = CreateDefaultSubobject<UNiagaraComponent>("TrailComponent");
	TrailComp->SetupAttachment(MeshComp);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = MeshComp;
	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->MaxSpeed = 6000.0f;
	ProjectileMovement->bShouldBounce = true;
}

void ANearGrenade::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANearGrenade, TrailColor);
}

// Called when the game starts or when spawned
void ANearGrenade::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(FuseHandle, this, &ANearGrenade::Explode, FuseTime, false);
}

void ANearGrenade::OnRep_TrailColor()
{
	VOID_IS_VALID(this);
	TrailComp->SetNiagaraVariableLinearColor("User.TeamColor", TrailColor);
}

FBuffInfo ANearGrenade::GetGrenadeInfo() const
{
	return FBuffInfo(Cast<UBaseBuff>(BuffType->GetDefaultObject()));
}

void ANearGrenade::SetGameMode(AGameModeBase* GM)
{
	VOID_IS_VALID(this);
	GameMode = GM;
}

void ANearGrenade::SetTeam(ETeam PlayerTeam)
{
	VOID_IS_VALID(this);
	if (!bOnAllPlayers)
	{
		InstigatorTeam = PlayerTeam;
	}
}

void ANearGrenade::SetGrenadeOwner(ATestPlayerState* OwnerState)
{
	VOID_IS_VALID(this);
	GrenadeOwner = OwnerState;
}

void ANearGrenade::SetTrailColor(FLinearColor PlayerColor)
{
	VOID_IS_VALID(this);
	ServerSetTrailColor(PlayerColor);
	TrailColor = PlayerColor;
}

void ANearGrenade::Explode()
{
	VOID_IS_VALID(this);
	if (BuffType)
	{
		if (ATestGameMode* TestGM = Cast<ATestGameMode>(GameMode))
		{
			UE_LOG(LogTemp, Warning, TEXT("Explosion! Owner %d"), GrenadeOwner != nullptr);
			UBuffsFunctionLib::GrenadeExplosionAtLocation(this, InstigatorTeam, false, bWhenVisible, GrenadeOwner);
		}
	}

	ServerSpawnBurst();

	CallDestroy();
}

void ANearGrenade::CallDestroy()
{
	VOID_IS_VALID(this);
	Destroy();
}

void ANearGrenade::SpawnExplosionSFX_Implementation()
{
}

void ANearGrenade::MultiSpawnBurst_Implementation()
{
	VOID_IS_VALID(this);
	SpawnExplosionSFX();
	FVector SpawnLoc = MeshComp->GetComponentLocation();
	if (BurstEffect)
	{
		UNiagaraComponent* NewComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BurstEffect, SpawnLoc);
		if (NewComponent)
        {
        	NewComponent->SetNiagaraVariableLinearColor("User.PartColor", GetGrenadeInfo().FXColor);
        }
	}
	if (ExplosionSFX)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ExplosionSFX, SpawnLoc);
	}
}

void ANearGrenade::ServerSpawnBurst_Implementation()
{
	VOID_IS_VALID(this);
	MultiSpawnBurst();
}

void ANearGrenade::ServerSetTrailColor_Implementation(FLinearColor PlayerColor)
{
	VOID_IS_VALID(this);
	if (TrailComp)
	{
		TrailComp->SetNiagaraVariableLinearColor("User.TeamColor", PlayerColor);
	}
}
