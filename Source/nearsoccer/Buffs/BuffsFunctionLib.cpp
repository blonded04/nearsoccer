// Nearsoccer 2022


#include "BuffsFunctionLib.h"

#include <complex>

#include "BaseBuff.h"
#include "NearGrenade.h"
#include "Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"
#include "TestPlayerState.h"
#include "WallRunnerCharacter.h"

void UBuffsFunctionLib::ApplyBuffAtLocation(TSubclassOf<UBaseBuff> BuffClass,
                                            FVector Location,
                                            ETeam InstigatorTeam,
                                            bool bIsOnInstigatorTeam,
                                            ATestGameMode* GameMode,
                                            USoundBase* SoundToSpawn)
{
	float Radius = Cast<UBaseBuff>(BuffClass->GetDefaultObject())->GetRadius();
	for (APlayerController* Controller : GameMode->GetAllPlayers())
	{
		if (AWallRunnerCharacter* Player = Cast<AWallRunnerCharacter>(Controller->GetCharacter()))
		{
			if (ATestPlayerState* PState = Cast<ATestPlayerState>(Controller->PlayerState))
			{
				if ((PState->GetTeamType() == InstigatorTeam) == bIsOnInstigatorTeam)
				{
					FVector Distance = Player->GetActorLocation() - Location;
					if (Distance.Size() <= Radius)
					{
						Player->SpawnUISound(SoundToSpawn, 0.0f);
						Player->BuffComp->AddBuff(BuffClass, Distance);
						
					}
				}
			}
		}
	}
}

bool UBuffsFunctionLib::DoLineTrace(AActor* StartActor, AWallRunnerCharacter* Target, UWorld* World, bool bWhenVisible)
{
	FHitResult HitRes;
	FVector End = Target->GetActorLocation();
	if (bWhenVisible)
	{
		End = Target->FPSCameraComponent->GetComponentLocation();
	}
	FVector Line = End - StartActor->GetActorLocation();
	FVector Start = StartActor->GetActorLocation() + (Line / Line.Size() * 100.0f);
	FCollisionObjectQueryParams QueryParams;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(StartActor);
	AGameModeBase* GM = World->GetAuthGameMode();
	if (ATestGameMode* GameMode = Cast<ATestGameMode>(GM))
	{
		for (APlayerController* Controller : GameMode->GetAllPlayers())
		{
			ACharacter* Character = Controller->GetCharacter();
			if (Character != Target)
			{
				CollisionQueryParams.AddIgnoredActor(Character);
			}
		}
	}
	World->LineTraceSingleByObjectType(HitRes, Start, End, QueryParams, CollisionQueryParams);
	//DrawDebugLine(World, Start, End, FColor::Yellow, false, 30.0f, 0, 5.0f);
	//DrawDebugLine(World, StartActor->GetActorLocation(), Start, FColor::Red, false, 30.0f, 0, 5.0f);
	UE_LOG(LogTemp, Warning, TEXT("HitActor: %p"), HitRes.GetActor());
	if (HitRes.GetActor() == Target)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Blocked"));
		return false;
	}
	UE_LOG(LogTemp, Warning, TEXT("Blocked"));
	return true;
}

bool UBuffsFunctionLib::CheckIsVisible(AActor* Grenade, AWallRunnerCharacter* Target, UWorld* World)
{
	FVector TargetLoc = Target->FPSCameraComponent->GetComponentLocation();
	FVector GrenadeLoc = Grenade->GetActorLocation();
	FRotator ToGrenadeRot = UKismetMathLibrary::FindLookAtRotation(TargetLoc, GrenadeLoc);
	FRotator CameraRot = Target->FPSCameraComponent->GetComponentRotation();
	float DeltaZ = CameraRot.Yaw - ToGrenadeRot.Yaw;
	float DeltaX = CameraRot.Pitch - ToGrenadeRot.Pitch;
	UE_LOG(LogTemp, Warning, TEXT("Delta: %f Z, %f X"), DeltaZ, DeltaX);
	if (!(DeltaZ <= 60 && DeltaZ >= -60 && DeltaX <= 45 && DeltaX >= -45))
	{
		return false;
	}
	else
	{
		return true;
	}
}


void UBuffsFunctionLib::GrenadeExplosionAtLocation(ANearGrenade* Grenade,
                                                   ETeam InstigatorTeam,
                                                   bool bIsOnInstigatorTeam,
                                                   bool bWhenVisible, ATestPlayerState* GrenadeInstigator)
{
	UWorld* World = Grenade->GetWorld();
	TSubclassOf<UBaseBuff> BuffClass = Grenade->GetBuffType();
	FVector Location = Grenade->GetActorLocation();
	AGameModeBase* GM = World->GetAuthGameMode();
	if (ATestGameMode* GameMode = Cast<ATestGameMode>(GM))
	{
		float Radius = Cast<UBaseBuff>(BuffClass->GetDefaultObject())->GetRadius();
		for (APlayerController* Controller : GameMode->GetAllPlayers())
		{
			if (AWallRunnerCharacter* Player = Cast<AWallRunnerCharacter>(Controller->GetCharacter()))
			{
				UE_LOG(LogTemp, Warning, TEXT("Checking %p"), Player);
				if (ATestPlayerState* PState = Cast<ATestPlayerState>(Controller->PlayerState))
				{
					if ((PState->GetTeamType() == InstigatorTeam) == bIsOnInstigatorTeam)
					{
						const bool bIsVisible = !bWhenVisible || (bWhenVisible &&
							CheckIsVisible(Grenade, Player, World));
						const bool bIsNear = !DoLineTrace(Grenade, Player, World, bWhenVisible);
						FVector Distance = Player->GetActorLocation() - Location;
						const bool bIsValid = Distance.Size() <= Radius && bIsNear && bIsVisible;
						float SuperDist = std::min(Radius, 150.0f);
						UE_LOG(LogTemp, Warning, TEXT("Distance: %f, Superradius: %f"), Distance.Size(), SuperDist);
						const bool bIsSuperValid = Distance.Size() <= SuperDist;
						if (bIsValid || bIsSuperValid)
						{
							Player->BuffComp->AddBuff(BuffClass, Distance, GrenadeInstigator);
						}
					}
				}
			}
		}
	}
}
