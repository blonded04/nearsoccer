// Nearsoccer 2022


#include "Components/Abilities/BuffComponent.h"
#include "BaseBuff.h"
#include "WallRunnerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UBuffComponent::UBuffComponent()
{
	// ...
}


// Called when the game starts
void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...
}

void UBuffComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// Replicate to everyone
	DOREPLIFETIME(UBuffComponent, BuffsInfoArray);
	DOREPLIFETIME(UBuffComponent, bIsHasteActivated);
	DOREPLIFETIME(UBuffComponent, bIsBounceActivated);
	DOREPLIFETIME(UBuffComponent, bIsShieldActivated);
	DOREPLIFETIME(UBuffComponent, bIsGunPowerActivated);
	DOREPLIFETIME(UBuffComponent, bIsFlashed);
	DOREPLIFETIME(UBuffComponent, bIsSlowed);
	DOREPLIFETIME(UBuffComponent, bIsFreeze);
	DOREPLIFETIME(UBuffComponent, MoveSpeedMultiplier);
}

void UBuffComponent::AddBuff(TSubclassOf<UBaseBuff> BuffClass, FVector DistanceFromSource, ATestPlayerState* BuffInstigator)
{
	UBaseBuff* Buff = NewObject<UBaseBuff>(this, BuffClass);
	Buff->SetDistance(DistanceFromSource);
	Buff->SetBuffInstigator(BuffInstigator);
	UE_LOG(LogTemp, Warning, TEXT("New Buff: %s"), *Buff->GetName());
	for (UBaseBuff* ExistingBuff : BuffsArray)
	{
		if (ExistingBuff->GetClass() == BuffClass)
		{
			if (ExistingBuff->IsResets())
			{
				ExistingBuff->ResetTimer();
			}
			else
			{
				ExistingBuff->Activate(Cast<ACharacter>(GetOwner()));
			}
			if (AWallRunnerCharacter* WRPlayer = Cast<AWallRunnerCharacter>(GetOwner()))
			{
				WRPlayer->SpawnFX(FBuffInfo(Buff));
			}
			return;
		}
	}
	Buff->Activate(Cast<ACharacter>(GetOwner()));
	BuffsArray.Add(Buff);
	BuffsInfoArray.Add(FBuffInfo(Buff));
	if (AWallRunnerCharacter* WRPlayer = Cast<AWallRunnerCharacter>(GetOwner()))
	{
		WRPlayer->SpawnFX(FBuffInfo(Buff));
	}
}

void UBuffComponent::RemoveBuff(UBaseBuff* Buff)
{
	for (int Index = 0; Index < BuffsInfoArray.Num(); ++Index)
	{
		if (BuffsInfoArray[Index].BuffName == Buff->GetName())
		{
			BuffsInfoArray.RemoveAt(Index);
		}
	}
	BuffsArray.Remove(Buff);
}

FString UBuffComponent::GetBuffsStatus() const
{
	FString Status = " ";
	for (FBuffInfo BuffInfo : BuffsInfoArray)
	{
		Status += BuffInfo.BuffName + "; ";
	}
	if (Status == " ")
	{
		Status += "None";
	}
	return Status;
}

void UBuffComponent::SetIsHasteActivated(float HasteMulti, bool Value)
{
	VOID_IS_VALID(GetOwner());
	if (!GetOwner()->HasAuthority())
	{
		ServerSetIsHasteActivated(HasteMulti, Value);
	}
	else
	{
		if (Value)
		{
			MoveSpeedMultiplier *= HasteMulti;
		}
		else
		{
			MoveSpeedMultiplier /= HasteMulti;
		}
		UE_LOG(LogTemp, Warning, TEXT("New MoveSpeedMultiplier: %f"), MoveSpeedMultiplier);
		bIsHasteActivated = Value;
	}
}

void UBuffComponent::SetJumpVelocity(float BounceMulti, bool IsActivating)
{
	VOID_IS_VALID(GetOwner());
	if (!GetOwner()->HasAuthority())
	{
		ServerSetJumpVelocity(BounceMulti, IsActivating);
	}
	else
	{
		bIsBounceActivated = IsActivating;
		ACharacter* Player = Cast<ACharacter>(GetOwner());
		Player->GetCharacterMovement()->JumpZVelocity *= BounceMulti;
		UE_LOG(LogTemp, Warning, TEXT("New JumpZVelocity: %f"), Player->GetCharacterMovement()->JumpZVelocity);
	}
}

void UBuffComponent::SetIsShieldActivated(bool Value)
{
	VOID_IS_VALID(GetOwner());
	if (!GetOwner()->HasAuthority())
	{
		ServerSetIsShieldActivated(Value);
	}
	else
	{
		bIsShieldActivated = Value;
		ACharacter* Owner = Cast<ACharacter>(GetOwner());
		if (AWallRunnerCharacter* WRPlayer = Cast<AWallRunnerCharacter>(Owner))
		{
			if (WRPlayer->GetIsInLava() && Value == 0)
			{
				WRPlayer->GetHurt(true);
			}
		}
	}
}

void UBuffComponent::SetIsGunPowerActivated(bool Value)
{
	VOID_IS_VALID(GetOwner());
	if (!GetOwner()->HasAuthority())
	{
		ServerSetIsGunPowerActivated(Value);
	}
	else
	{
		bIsGunPowerActivated = Value;
	}
}

void UBuffComponent::SetIsFlashed(bool Value)
{
	VOID_IS_VALID(GetOwner());
	if (!GetOwner()->HasAuthority())
	{
		ServerSetIsFlashed(Value);
	}
	else
	{
		bIsFlashed = Value;
	}
}

void UBuffComponent::SetIsSlowed(float SlowMulti, bool Value)
{
	VOID_IS_VALID(GetOwner());
	if (!GetOwner()->HasAuthority())
	{
		ServerSetIsSlowed(SlowMulti, Value);
	}
	else
	{
		if (Value)
		{
			MoveSpeedMultiplier *= SlowMulti;
		}
		else
		{
			MoveSpeedMultiplier /= SlowMulti;
		}
		UE_LOG(LogTemp, Warning, TEXT("New MoveSpeedMultiplier: %f"), MoveSpeedMultiplier);
		bIsSlowed = Value;
	}
}

void UBuffComponent::SetIsFreeze(bool Value)
{
	VOID_IS_VALID(GetOwner());
	if (!GetOwner()->HasAuthority())
	{
		ServerSetIsFlashed(Value);
	}
	else
	{
		bIsFreeze = Value;
		ACharacter* Owner = Cast<ACharacter>(GetOwner());
		Owner->GetCharacterMovement()->StopMovementImmediately();
		if (AWallRunnerCharacter* WRPlayer = Cast<AWallRunnerCharacter>(Owner))
		{
			WRPlayer->StopRiding();
		}
	}
}

void UBuffComponent::ServerSetIsHasteActivated_Implementation(float HasteMulti, bool Value)
{
	VOID_IS_VALID(GetOwner());
	if (GetOwner()->HasAuthority())
	{
		SetIsHasteActivated(HasteMulti, Value);
	}
}

void UBuffComponent::ServerSetJumpVelocity_Implementation(float BounceMulti, bool IsActivating)
{
	VOID_IS_VALID(GetOwner());
	if (GetOwner()->HasAuthority())
	{
		SetJumpVelocity(BounceMulti, IsActivating);
	}
}

void UBuffComponent::ServerSetIsShieldActivated_Implementation(bool Value)
{
	VOID_IS_VALID(GetOwner());
	if (GetOwner()->HasAuthority())
	{
		SetIsShieldActivated(Value);
	}
}

void UBuffComponent::ServerSetIsGunPowerActivated_Implementation(bool Value)
{
	VOID_IS_VALID(GetOwner());
	if (GetOwner()->HasAuthority())
	{
		SetIsGunPowerActivated(Value);
	}
}

void UBuffComponent::ServerSetIsFlashed_Implementation(bool Value)
{
	VOID_IS_VALID(GetOwner());
	if (GetOwner()->HasAuthority())
	{
		SetIsFlashed(Value);
	}
}

void UBuffComponent::ServerSetIsSlowed_Implementation(float SlowMulti, bool Value)
{
	VOID_IS_VALID(GetOwner());
	if (GetOwner()->HasAuthority())
	{
		SetIsSlowed(SlowMulti, Value);
	}
}

void UBuffComponent::ServerSetIsFreeze_Implementation(bool Value)
{
	VOID_IS_VALID(GetOwner());
	if (GetOwner()->HasAuthority())
	{
		SetIsFreeze(Value);
	}
}
