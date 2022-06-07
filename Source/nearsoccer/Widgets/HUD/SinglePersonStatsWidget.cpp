// Nearsoccer 2022


#include "SinglePersonStatsWidget.h"

#include "TestPlayerState.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"

void USinglePersonStatsWidget::Update(APlayerState* PlayerState,
                                      const TArray<TEnumAsByte<EStatisticsType>>& DisplayableStats)
{
	PlayerName->SetText(FText::FromString(PlayerState->GetPlayerName()));

	StatsBox->ClearChildren();
	auto PState = Cast<ATestPlayerState>(PlayerState);
	for (auto key : DisplayableStats)
	{
		float val = PState->GetStat(key);
		UTextBlock* TextBlock = NewObject<UTextBlock>();
		if (key == READY_STATUS)
		{
			TextBlock->SetText(FText::FromString((val > 0) ? "Ready" : "Not ready"));
		}
		else
		{
			TextBlock->SetText(FText::AsNumber(val));
		}
		TextBlock->SetJustification(ETextJustify::Left);

		auto* slot = StatsBox->AddChild(TextBlock);
		Cast<UHorizontalBoxSlot>(slot)->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	}
	score = Cast<ATestPlayerState>(PlayerState)->GetPlayerScore();
}

void USinglePersonStatsWidget::NativeConstruct()
{
	Super::NativeConstruct();
}
