// Nearsoccer 2022


#include "Widgets/HUD/CooldownWidget.h"

#include "Components/Abilities/DashComponent.h"
#include "NearsoccerCore.h"
#include "WallRunnerCharacter.h"
#include "Components/ProgressBar.h"

void UCooldownWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bCasted && GetOwningPlayer() != nullptr && GetOwningPlayer()->GetCharacter() != nullptr)
	{
		AWallRunnerCharacter* Player = Cast<AWallRunnerCharacter>(GetOwningPlayer()->GetCharacter());
		if (Player != nullptr)
		{
			bCasted = true;
			Component = Player->DashComponent;
		}
	}
	
	if (bCasted)
	{
		VOID_IS_VALID(Component);
		
		Progress = std::max(0.0f, Progress - InDeltaTime);
		if (Component->isDashedRecently())
		{
			Progress = Component->DashCooldown;
			Component->ServerDashProcessed(Component, false);
		}
		SetDashCooldown();
	}
}

void UCooldownWidget::SetDashCooldown()
{
	VOID_IS_VALID(Component)
	VOID_IS_VALID(DashCooldownBar)
	
	if (DashCooldownBar != nullptr && Component != nullptr)
	{
		DashCooldownBar->SetPercent(Progress / Component->DashCooldown);
	}
}
