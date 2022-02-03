// Copyright Epic Games, Inc. All Rights Reserved.

#include "nearsoccerGameMode.h"
#include "nearsoccerHUD.h"
#include "nearsoccerCharacter.h"
#include "UObject/ConstructorHelpers.h"

AnearsoccerGameMode::AnearsoccerGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AnearsoccerHUD::StaticClass();
}
