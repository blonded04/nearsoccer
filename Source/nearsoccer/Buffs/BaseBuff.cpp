// Nearsoccer 2022


#include "BaseBuff.h"
#include "Core/NearsoccerCore.h"
#include "Components/Abilities/BuffComponent.h"
#include "Components/BrushComponent.h"
#include "GameFramework/Character.h"

UBaseBuff::UBaseBuff()
{
}

FString UBaseBuff::GetName() const
{
	VALUE_IS_VALID(this, "");
	return BuffName;
}

float UBaseBuff::GetRadius() const
{
	VALUE_IS_VALID(this, 0.0f);
	return BuffRadius;
}

void UBaseBuff::Activate_Implementation(ACharacter* Character)
{
	BuffCharacter = Character;
	BuffCharacter->GetWorld()->GetTimerManager().ClearTimer(BuffHandle);
	Character->GetWorld()->GetTimerManager().SetTimer(BuffHandle, this, &UBaseBuff::Deactivate, BuffTime, false);
}

void UBaseBuff::Deactivate_Implementation()
{
	VOID_IS_VALID(GetOuter());
	VOID_IS_VALID(BuffCharacter);
	Cast<UBuffComponent>(GetOuter())->RemoveBuff(this);
	BuffCharacter->GetWorld()->GetTimerManager().ClearTimer(BuffHandle);
	BuffCharacter = nullptr;
}

UBuffComponent* UBaseBuff::GetBuffComponent()
{
	VALUE_IS_VALID(GetOuter(), nullptr);
	return Cast<UBuffComponent>(GetOuter());
}

void UBaseBuff::ResetTimer()
{
	VOID_IS_VALID(BuffCharacter);
	BuffCharacter->GetWorld()->GetTimerManager().ClearTimer(BuffHandle);
	BuffCharacter->GetWorld()->GetTimerManager().SetTimer(BuffHandle, this, &UBaseBuff::Deactivate, BuffTime, false);
}

void UBaseBuff::SetDistance(FVector Distance)
{
	BuffDistanceFromSource = Distance;
}

void UBaseBuff::SetBuffInstigator(ATestPlayerState* PlayerState)
{
	BuffInstigator = PlayerState;
}
