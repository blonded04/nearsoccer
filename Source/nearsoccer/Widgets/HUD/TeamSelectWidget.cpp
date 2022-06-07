// Nearsoccer 2022


#include "Widgets/HUD/TeamSelectWidget.h"

#include "TestGameState.h"
#include "TestPlayerState.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UTeamSelectWidget::Draw(ETeam CurrentTeam)
{
	if (APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		auto PState = PController->GetPlayerState<ATestPlayerState>();
		if (CurrentTeam == ETeam::ENone)
		{
			CurrentTeam = PState->GetTeamType();
		}
		switch (CurrentTeam)
		{
		case ETeam::EBlue:
			RedButtonText->SetText(FText::FromString("Join"));
			JoinRed->SetIsEnabled(true);
			BlueButtonText->SetText(FText::FromString("Your team"));
			JoinBlue->SetIsEnabled(false);
			break;
		case ETeam::ERed:
			BlueButtonText->SetText(FText::FromString("Join"));
			JoinBlue->SetIsEnabled(true);
			RedButtonText->SetText(FText::FromString("Your team"));
			JoinRed->SetIsEnabled(false);
			break;
		}
		NameBox->SetText(FText::FromString(PState->GetPlayerName()));

		auto GState = Cast<ATestGameState>(GetWorld()->GetGameState());

		BlueTeamCount->SetText(FText::FromString(FString::FromInt(GState->BlueTeam->GetPlayersCount()) + "/4"));
		RedTeamCount->SetText(FText::FromString(FString::FromInt(GState->RedTeam->GetPlayersCount()) + "/4"));
	}
}

bool UTeamSelectWidget::Initialize()
{
	auto SuperRes = Super::Initialize();


	Draw();

	if (SaveName)
	{
		SaveName->OnClicked.AddDynamic(this, &UTeamSelectWidget::OnSaveNameClicked);
	}

	if (JoinRed)
	{
		JoinRed->OnClicked.AddDynamic(this, &UTeamSelectWidget::OnJoinButtonClicked);
	}
	if (JoinBlue)
	{
		JoinBlue->OnClicked.AddDynamic(this, &UTeamSelectWidget::OnJoinButtonClicked);
	}

	return SuperRes;
}

void UTeamSelectWidget::OnJoinButtonClicked()
{
	if (APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		auto Character = Cast<AWallRunnerCharacter>(PController->GetCharacter());
		Character->ChangeTeamServer();
		Character->ToggleTeamSelect();
	}
}

void UTeamSelectWidget::OnSaveNameClicked()
{
	if (NameBox)
	{
		if (APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			Cast<AWallRunnerCharacter>(PController->GetCharacter())->ChangeName(NameBox->GetText().ToString());
		}
	}
}
