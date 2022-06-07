// Nearsoccer 2022


#include "Widgets/HUD/GameInfoWidget.h"
#include "Blueprint/WidgetTree.h"

#include "BuffEffectsWidget.h"

void UGameInfoWidget::Reset()
{
	for (auto TextWidget : {BlueScoreText, RedScoreText, TimeText, ErrorMessage, InfoMessage})
	{
		if (TextWidget)
		{
			TextWidget->SetText(FText::FromString(""));
		}
	}
	if (KillFeed)
	{
		KillFeed->ClearChildren();
	}
}

void UGameInfoWidget::DisableErrorMessage()
{
	if (ErrorTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ErrorTimerHandle);
	}
	if (ErrorMessage)
	{
		ErrorMessage->SetText(FText::FromString(""));
	}
}

void UGameInfoWidget::DisableInfoMessage()
{
	if (InfoTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(InfoTimerHandle);
	}
	if (InfoMessage)
	{
		InfoMessage->SetText(FText::FromString(""));
	}
}

void UGameInfoWidget::ChangeScoreText(ETeam Team, int NewScore)
{
	switch (Team)
	{
	case ETeam::EBlue:
		BlueScoreText->SetText(FText::AsNumber(NewScore));
		break;
	case ETeam::ERed:
		RedScoreText->SetText(FText::AsNumber(NewScore));
		break;
	}
}

void UGameInfoWidget::ChangeTimeText(int TimeLeft)
{
	int Minutes = TimeLeft / 60;
	int Seconds = TimeLeft % 60;
	auto MinutesText = FString::FromInt(Minutes);
	auto SecondsText = FString::FromInt(Seconds);
	if (Minutes < 10)
	{
		MinutesText = "0" + MinutesText;
	}
	if (Seconds < 10)
	{
		SecondsText = "0" + SecondsText;
	}
	auto FinalText = MinutesText + ":" + SecondsText;
	TimeText->SetText(FText::FromString(FinalText));
}

void UGameInfoWidget::SetErrorMessage(const FString& Message)
{
	if (!ErrorMessage)
	{
		return;
	}
	ErrorMessage->SetText(FText::FromString(Message));

	if (ErrorTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ErrorTimerHandle);
	}


	GetWorld()->GetTimerManager().SetTimer(ErrorTimerHandle, this,
	                                       &UGameInfoWidget::DisableErrorMessage, ERROR_TIME, true);
}

void UGameInfoWidget::SetInfoMessage(const FString& Message)
{
	if (!InfoMessage)
	{
		return;
	}
	InfoMessage->SetText(FText::FromString(Message));

	if (InfoTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(InfoTimerHandle);
	}


	GetWorld()->GetTimerManager().SetTimer(InfoTimerHandle, this,
	                                       &UGameInfoWidget::DisableInfoMessage, INFO_TIME, true);
}

void UGameInfoWidget::PushToKillFeed(const FString& Killer, const FString& Killed, EDeathType DeathType,
                                     bool bWasKilled)
{
	if (!KillFeed)
		return;


	FString Message = "";
	switch (DeathType)
	{
	case ELava:
		if (bWasKilled)
		{
			Message = Killer + " pushed " + Killed + " into the lava";
		}
		else
		{
			Message = Killed + " jumped in lava (lol)";
		}
		break;
	case EGrenade:
		if (bWasKilled)
		{
			Message = Killer + " killed " + Killed + " with a grenade";
		}
		else
		{
			Message = Killed + " blew himself up (noob)";
		}
		break;
	}
	auto* TextWidget = WidgetTree->ConstructWidget<URichTextBlock>(TextBlockClass);
	TextWidget->SetText(FText::FromString(Message));

	KillFeed->AddChild(TextWidget);

	FTimerDelegate TimerCallback;
	TimerCallback.BindLambda([&]
	{
		if (KillFeed)
		{
			int count = KillFeed->GetChildrenCount();
			if (count > 0)
			{
				KillFeed->RemoveChildAt(KillFeed->GetChildrenCount() - 1);
			}
		}
	});

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, TimerCallback, KILLFEED_TIME, false);
}

void UGameInfoWidget::SetWinner(EWinnerType Winner)
{
	FString Text;
	switch (Winner)
	{
	case EWinnerType::EDraw: // Deal with it.
	case EWinnerType::EBlue:
		Text = "<Blue>TEAM LIZARD WINS</>";
		break;
	case EWinnerType::ERed:
		Text = "<Red>TEAM BEAR WINS</>";
		break;
	}
	Reset();
	WinnerText->SetText(FText::FromString(Text));
}
