// Nearsoccer 2022


#include "Widgets/HUD/SpeedWidget.h"

#include "Core/NearsoccerCore.h"
#include "GameFramework/Character.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"

void USpeedWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (GetOwningPlayerPawn() != nullptr)
	{
		SpeedInfo->SetText(FText::Format(FTextFormat::FromString("{0} m/s"), GetFloatAsTextWithPrecision(GetOwningPlayerPawn()->GetVelocity().Size() / 100.0f, 1)));
	}
}

