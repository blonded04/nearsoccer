// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "Core/NearsoccerCore.h"
#include "TeamPlayerStart.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EStartType: uint8
{
	HUB,
	GAMEFIELD,
	DISABLED,
	ENDGAME
};

UCLASS()
class NEARSOCCER_API ATeamPlayerStart : public APlayerStart
{
	GENERATED_BODY()

public:
	// Returns team
	UFUNCTION(BlueprintCallable)
	ETeam GetTeam();

	UFUNCTION(BlueprintCallable)
	EStartType GetType();

	UFUNCTION(BlueprintCallable)
	void SetType(EStartType Type);

protected:
	UPROPERTY(EditAnywhere, Category="Team")
	ETeam Team = ETeam::ENone;

	UPROPERTY(EditAnywhere, Category="Team")
	EStartType StartType = EStartType::HUB;
};
