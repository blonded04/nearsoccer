// Nearsoccer 2022


#include "Actors/ObjectsWithPhysics/NearsoccerSoccerBall.h"

#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/WallRunnerController.h"
#include "TestGameState.h"
#include "Net/UnrealNetwork.h"
#include "Runtime/Engine/Public/EngineUtils.h"

// Sets default values
ANearsoccerSoccerBall::ANearsoccerSoccerBall()
{
	BallTeam = ETeam::ENone;

	BallPoints = 300;

	BallPointsDecreaseValue = 1;

	DeltaSeconds = 1;
	DeltaSeconds /= 60;

	SpawnGate = nullptr;
}

// Called when the game starts or when spawned
void ANearsoccerSoccerBall::BeginPlay()
{
	Super::BeginPlay();

	UClass* SpawnPointClass = ANearSoccerGate::StaticClass();
	for (TActorIterator<AActor> Actor(GetWorld(), SpawnPointClass); Actor; ++
	     Actor)
	{
		ANearSoccerGate* GateActor = Cast<ANearSoccerGate>(*Actor);
		if (GateActor->GetTeam() != BallTeam)
		{
			EnemyGates.Add(GateActor);
		}
		else
		{
			SpawnGate = GateActor;
		}
	}

	GetWorld()->GetTimerManager().SetTimer(TickHandle, this,
	                                       &ANearsoccerSoccerBall::BallTick, DeltaSeconds, true);

	SetLastPlayerPoked(nullptr);
	SetLastPlayerBlocked(nullptr);
}

void ANearsoccerSoccerBall::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANearsoccerSoccerBall, LastPlayerPoked);
	DOREPLIFETIME(ANearsoccerSoccerBall, LastPlayerBlocked);
}

ETeam ANearsoccerSoccerBall::GetTeam()
{
	VALUE_IS_VALID(this, ETeam::ENone);
	return BallTeam;
}

void ANearsoccerSoccerBall::SetLastPlayerPoked_Implementation(AWallRunnerController* Val)
{
	VOID_IS_VALID(this);

	if (Val == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("SoccerBall: %p's last player poked was set to NULL"), this);
	}
	if (Val != LastPlayerPoked)
	{
		LastPlayerPoked = Val;
	}
}

void ANearsoccerSoccerBall::SetLastPlayerBlocked_Implementation(AWallRunnerController* Val)
{
	VOID_IS_VALID(this);

	if (Val == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("SoccerBall: %p's last player blocked was set to NULL"), this);
	}

	LastPlayerBlocked = Val;
}

AWallRunnerController* ANearsoccerSoccerBall::GetLastPlayerPoked() const
{
	VALUE_IS_VALID(this, nullptr);

	return LastPlayerPoked;
}

AWallRunnerController* ANearsoccerSoccerBall::GetLastPlayerBlocked() const
{
	VALUE_IS_VALID(this, nullptr);

	return LastPlayerBlocked;
}

bool ANearsoccerSoccerBall::IsGateNear(ANearSoccerGate* GateActor)
{
	VALUE_IS_VALID(this, false);
	FVector GateLoc = GateActor->GetActorLocation();
	FVector BallLoc = GetActorLocation();
	float Distance = (BallLoc - GateLoc).Size();
	if (Distance > GateActor->GetRadius())
	{
		return false;
	}
	return true;
}

void ANearsoccerSoccerBall::TransferPoints(ETeam ToTeam)
{
	int ToDecrease = std::min(BallPointsDecreaseValue, BallPoints);
	BallPoints -= ToDecrease;
	UE_LOG(LogTemp, Warning, TEXT("Decreased by %d; %d pts remains"), ToDecrease, BallPoints);

	Cast<ATestGameState>(GetWorld()->GetGameState())->ServerChangeBallScore(ToTeam, ToDecrease);

	if (LastPlayerPoked)
	{
		auto PlayerState = LastPlayerPoked->GetPlayerState<ATestPlayerState>();
		if (PlayerState->GetTeamType() == ToTeam) // cool
		{
			PlayerState->ModifyStat(EStatisticsType::GOALS_SCORED, 1);
		}
		else // noob
		{
		}
	}
}

void ANearsoccerSoccerBall::CallDestroy()
{
	if (DestroyEffect)
	{
		SpawnBurst();
	}
	if (SpawnGate)
	{
		SpawnGate->SpawnBall();
	}
	this->Destroy();
}

void ANearsoccerSoccerBall::BallTick()
{
	VOID_IS_VALID(this);
	if (HasAuthority())
	{
		bool bIsNoPts = false;
		for (int Index = 0; Index < EnemyGates.Num(); ++Index)
		{
			ANearSoccerGate* GateActor = EnemyGates[Index];
			if (IsGateNear(GateActor))
			{
				TransferPoints(GateActor->GetTeam());

				if (BallPoints <= 0)
				{
					bIsNoPts = true;
				}
				break;
			}
		}
		if (bIsNoPts)
		{
			CallDestroy();
		}
	}
}

void ANearsoccerSoccerBall::SpawnBurst()
{
	ServerCallSpawnBurst();
}

void ANearsoccerSoccerBall::ServerCallSpawnBurst_Implementation()
{
	MultiSpawnBurst();
}

void ANearsoccerSoccerBall::MultiSpawnBurst_Implementation()
{
	VOID_IS_VALID(this);
	VOID_IS_VALID(DestroyEffect);

	FVector SpawnLoc = GetStaticMeshComponent()->GetComponentLocation();
	UNiagaraComponent* NewComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),
		DestroyEffect,SpawnLoc);
	if (NewComponent)
	{
		NewComponent->SetNiagaraVariableLinearColor("User.PartColor", ParticleColor);
	}
}

