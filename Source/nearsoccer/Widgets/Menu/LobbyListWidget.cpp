// Nearsoccer 2022


#include "Widgets/Menu/LobbyListWidget.h"

#include "NearsoccerCore.h"
#include "TestGameInstance.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void ULobbyListWidget::NativeConstruct()
{
	Super::NativeConstruct();
	Update();
}

void ULobbyListWidget::Draw(TArray<FOnlineSessionSearchResult>& Lobbies)
{
	VOID_IS_VALID(LobbyInfoClass);

	if (LoadingTimerHandler.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(LoadingTimerHandler);
	}
	if (LoadingText)
	{
		LoadingText->SetVisibility(ESlateVisibility::Hidden);
	}

	for (auto& lobby : Lobbies)
	{
		auto* widget = CreateWidget<ULobbyInfoWidget>(this, LobbyInfoClass);
		widget->Update(LobbyBox->GetChildrenCount() + 1, lobby);
		LobbyBox->AddChild(widget);
	}
}

void ULobbyListWidget::Update()
{
	if (LoadingTimerHandler.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(LoadingTimerHandler);
	}
	if (LoadingText)
	{
		LoadingText->SetVisibility(ESlateVisibility::Visible);
		LoadingText->SetText(FText::FromString("Loading"));

		GetWorld()->GetTimerManager().SetTimer(LoadingTimerHandler, this,
		                                       &ULobbyListWidget::LoadingTick, 1, true);
	}
	LobbyBox->ClearChildren();
	Cast<UTestGameInstance>(GetGameInstance())->ListLobbies(this);
}

void ULobbyListWidget::LoadingTick()
{
	if (LoadingText)
	{
		int NumDots = LoadingText->GetText().ToString().Len() - std::strlen("Loading");
		NumDots++;
		if (NumDots < 1 || NumDots > 6)
		{
			NumDots = 1;
		}
		FString NewText = "Loading";
		for (int i = 0; i < NumDots; i++)
		{
			NewText += ".";
		}
		LoadingText->SetText(FText::FromString(NewText));
	}
}
