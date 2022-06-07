// Nearsoccer 2022


#include "Actors/Weapons/GravityGunWeapon.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Actors/ObjectsWithPhysics/NearsoccerSoccerBall.h"
#include "Character/WallRunnerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Nearsoccer/Core/NearsoccerCore.h"
#include "Net/UnrealNetwork.h"


AGravityGunWeapon::AGravityGunWeapon()
{
	// Enable ticking for GrabberHandle to properly move to GrabbedLocation
	PrimaryActorTick.bCanEverTick = true;

	// Enable replication
	bReplicates = true;
}

// Setup on actor spawn
void AGravityGunWeapon::BeginPlay()
{
	Super::BeginPlay();

	// Setup member variables
	AWallRunnerCharacter* Plr = GetOwner<AWallRunnerCharacter>();
	VOID_IS_VALID(Plr);
	Camera = Plr->FPSCameraComponent;
	check(Camera != nullptr);

	// Attach to camera
	this->AttachToComponent(Camera, FAttachmentTransformRules::KeepRelativeTransform);
}

// Setup replication for member variables
void AGravityGunWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGravityGunWeapon, bCanPoke);
	DOREPLIFETIME(AGravityGunWeapon, bCanBlock);
}

// Ask gravity gun to poke object that is in LOS
void AGravityGunWeapon::PrimaryFireBegin_Implementation()
{
	VOID_IS_VALID(this);
	VOID_IS_VALID(Camera);

	UE_LOG(LogTemp, Warning, TEXT("GravityGun: %p asks to PrimaryFireBegin()"), this->GetOwner());

	if (!bCanPoke)
	{
		return;
	}

	if (FXOnShot)
	{
		SpawnNiagaraEffects(FXOnShot, ColorFXDefault);
		SpawnSoundEffect(SFXOnShot);
	}

	FHitResult Hit;
	float Range = GunSearchRange;
	if (GetOwner<AWallRunnerCharacter>()->BuffComp->CheckIsGunPowerActivated())
	{
		Range = GunSearchBuffedRange;
	}
	auto Obj = GetAimTarget(Range, Hit);

	if (Obj.IsSet())
	{
		if (FXOnPoke)
		{
			SpawnNiagaraEffects(FXOnPoke, ColorFXDefault, Hit.Distance);
			SpawnSoundEffect(SFXOnHit);
		}

		float Coef = 1.0f;
		if (GetOwner<AWallRunnerCharacter>()->BuffComp->CheckIsGunPowerActivated())
		{
			Coef *= LaunchBuffedCoefficient;
		}
		switch (Obj.GetValue().Key)
		{
		case ETargetType::Target_Ball:
			{
				Coef *= LaunchBallCoefficient;
				PokeObject(Obj.GetValue().Value, Coef * Camera->GetForwardVector());
			}
			break;
		case ETargetType::Target_Player:
			{
				Coef *= LaunchPlayerCoefficient;
				PokePlayer(Obj.GetValue().Value->GetOwner<AWallRunnerCharacter>(),
				           Coef * Camera->GetForwardVector());
			}
			break;
		}
	}
}

// Ask gravity gun to fully stop object that is in LOS
void AGravityGunWeapon::SecondaryFireBegin_Implementation()
{
	VOID_IS_VALID(this);
	VOID_IS_VALID(Camera);

	UE_LOG(LogTemp, Warning, TEXT("GravityGun: %p was asked to SecondaryFireBegin()"), this->GetOwner());

	if (!bCanBlock)
	{
		return;
	}

	if (FXOnShot)
	{
		SpawnNiagaraEffects(FXOnShot, ColorFXDefault);
		SpawnSoundEffect(SFXOnShot);
	}

	FHitResult Hit;
	float Range = GunSearchRange;
	if (GetOwner<AWallRunnerCharacter>()->BuffComp->CheckIsGunPowerActivated())
	{
		Range = GunSearchBuffedRange;
	}
	auto Obj = GetAimTarget(Range, Hit);

	if (Obj.IsSet() && Obj.GetValue().Key == ETargetType::Target_Ball)
	{
		if (FXOnBlock)
		{
			SpawnNiagaraEffects(FXOnBlock, ColorFXBlock, Hit.Distance);
			SpawnSoundEffect(SFXOnHit);
		}

		BlockObject(Obj.GetValue().Value);
	}
}

// Get primitive component in line of sight that is at least as close as Range
TOptional<TPair<AGravityGunWeapon::ETargetType, UPrimitiveComponent*>> AGravityGunWeapon::GetAimTarget(
	float Range, FHitResult& Hit) const
{
	VALUE_IS_VALID(this, {});
	VALUE_IS_VALID(Camera, {});
	VALUE_IS_VALID(GetOwner(), {});

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(GetOwner());
	if (GetWorld()->LineTraceSingleByChannel(
		Hit, Camera->GetComponentLocation(), Camera->GetComponentLocation() + FRotationMatrix(
			GetOwner<AWallRunnerCharacter>()->Controller->GetControlRotation()).GetScaledAxis(EAxis::X) * Range,
		ECC_Pawn, QueryParams))
	{
		UE_LOG(LogTemp, Warning, TEXT("GravityGun: [Trace] We've hit %s"), *Hit.GetActor()->GetName());

		if (Hit.GetComponent() != nullptr && Cast<ANearsoccerSoccerBall>(Hit.GetActor()) != nullptr)
		{
			return TPair<ETargetType, UPrimitiveComponent*>{ETargetType::Target_Ball, Hit.GetComponent()};
		}
		if (Hit.GetComponent() != nullptr && Cast<AWallRunnerCharacter>(Hit.GetActor()) != nullptr
			&& GetOwner<AWallRunnerCharacter>()->GetPlayerState<ATestPlayerState>()->GetTeamType() !=
			Cast<AWallRunnerCharacter>(Hit.GetActor())->GetPlayerState<ATestPlayerState>()->GetTeamType())
		{
			return TPair<ETargetType, UPrimitiveComponent*>{ETargetType::Target_Player, Hit.GetComponent()};
		}
	}
	return {};
}

// Pokes object and sets cooldown
void AGravityGunWeapon::PokeObject(UPrimitiveComponent* Obj, FVector Impulse)
{
	VOID_IS_VALID(this);
	VOID_IS_VALID(GetOwner());

	check(HasAuthority());

	if (bCanPoke)
	{
		Obj->GetOwner<ANearsoccerSoccerBall>()->SetLastPlayerPoked(
			GetOwner<AWallRunnerCharacter>()->GetController<AWallRunnerController>());
		Obj->AddImpulse(Impulse, NAME_None, true);
		bCanPoke = false;
		SetIndicatorOwnerNoSee(GetOwner<AWallRunnerCharacter>()->IndicatorPrimaryFire, true);
		GetWorld()->GetTimerManager().SetTimer(PokeCooldownHandle, this,
		                                       &AGravityGunWeapon::EnablePoking, PokeCooldown);
	}
}

// Pokes player and sets cooldown
void AGravityGunWeapon::PokePlayer(AWallRunnerCharacter* Plr, FVector Impulse)
{
	VOID_IS_VALID(this);
	VOID_IS_VALID(GetOwner());
	VOID_IS_VALID(Plr);

	check(HasAuthority());

	if (bCanPoke && !Plr->BuffComp->CheckIsShieldActivated())
	{
		auto PlrState = Cast<ATestPlayerState>(Plr->GetPlayerState());
		
		PlrState->SetLastPushed(GetOwner<AWallRunnerCharacter>()->GetController()->GetPlayerState<ATestPlayerState>());
		Plr->LaunchCharacter(Impulse, false, false);
		bCanPoke = false;
		SetIndicatorOwnerNoSee(GetOwner<AWallRunnerCharacter>()->IndicatorPrimaryFire, true);
		GetWorld()->GetTimerManager().SetTimer(PokeCooldownHandle, this,
		                                       &AGravityGunWeapon::EnablePoking, PokeCooldown);
	}
}

// Allows poking
void AGravityGunWeapon::EnablePoking_Implementation()
{
	VOID_IS_VALID(this);
	VOID_IS_VALID(GetOwner());

	check(HasAuthority());

	bCanPoke = true;
	SetIndicatorOwnerNoSee(GetOwner<AWallRunnerCharacter>()->IndicatorPrimaryFire, false);
}

// Blocks object
void AGravityGunWeapon::BlockObject(UPrimitiveComponent* Obj)
{
	VOID_IS_VALID(this);
	VOID_IS_VALID(GetOwner());

	check(HasAuthority());

	if (bCanBlock)
	{
		Obj->GetOwner<ANearsoccerSoccerBall>()->SetLastPlayerBlocked(
			GetOwner<AWallRunnerCharacter>()->GetController<AWallRunnerController>());
		Obj->AddImpulse(-Obj->GetComponentVelocity(), NAME_None, true);
		bCanBlock = false;
		SetIndicatorOwnerNoSee(GetOwner<AWallRunnerCharacter>()->IndicatorSecondaryFire, true);
		GetWorld()->GetTimerManager().SetTimer(BlockCooldownHandle, this,
		                                       &AGravityGunWeapon::EnableBlocking, BlockCooldown);
	}
}

// Allows blocking
void AGravityGunWeapon::EnableBlocking_Implementation()
{
	VOID_IS_VALID(this);
	VOID_IS_VALID(GetOwner());

	check(HasAuthority());

	bCanBlock = true;
	SetIndicatorOwnerNoSee(GetOwner<AWallRunnerCharacter>()->IndicatorSecondaryFire, false);
}

// Hides / shows ready-to-fire indicator on all clients
void AGravityGunWeapon::SetIndicatorOwnerNoSee_Implementation(UNiagaraComponent* Indicator, bool bNewOwnerNoSee)
{
	VOID_IS_VALID(this);
	VOID_IS_VALID(Indicator);
	
	Indicator->SetOwnerNoSee(bNewOwnerNoSee);
}

// Spawns Niagara effects on each client
void AGravityGunWeapon::SpawnNiagaraEffects_Implementation(UNiagaraSystem* Effect, FLinearColor Color,
                                                           float Len)
{
	VOID_IS_VALID(this);

	auto Player = GetOwner<AWallRunnerCharacter>();

	VOID_IS_VALID(Player);

	UNiagaraComponent* NewComponent;
	UMeshComponent* Mesh;

	if (Player->IsLocallyControlled())
	{
		Mesh = Player->FPSMesh;
	}
	else
	{
		Mesh = Player->TPSGunMesh;
	}

	NewComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
		Effect, Mesh, "Muzzle", FVector::ZeroVector, FRotator::ZeroRotator,
		EAttachLocation::KeepRelativeOffset,
		true);

	if (NewComponent)
	{
		NewComponent->SetNiagaraVariableVec3("User.Length", FVector{0.0f, 0.0f, Len});
		NewComponent->SetNiagaraVariableLinearColor("User.Color", Color);
	}
}

// Spawns sound effects on each client
void AGravityGunWeapon::SpawnSoundEffect_Implementation(USoundBase* Effect)
{
	VOID_IS_VALID(this);

	auto Player = GetOwner<AWallRunnerCharacter>();
	VOID_IS_VALID(Player);

	FVector SpawnLoc;
	if (Player->IsLocallyControlled())
	{
		SpawnLoc = Player->FPSMesh->GetComponentLocation();
	}
	else
	{
		SpawnLoc = Player->TPSGunMesh->GetComponentLocation();
	}

	if (Effect)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), Effect, SpawnLoc);
	}
}