// Nearsoccer 2022


#include "Widgets/HUD/SpawnTimerWidget.h"

#include "NearsoccerCore.h"

void USpawnTimerWidget::SetTimeLeft(int TimeLeft)
{
	VOID_IS_VALID(TimerText);
	FString Text = "Game starts in " + FString::FromInt(TimeLeft);
	TimerText->SetText(FText::FromString(Text));
}
