// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerStatsComponent.generated.h"


// SCORE should always be last, Indexes must be consecutive numbers statring from 0.
UENUM()
enum EStatisticsType { KILLS=0, DEATHS=1, ASSISTS=2, GOALS_SCORED=3, GOALS_SAVED=4, SCORE=5, READY_STATUS=6 };

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NEARSOCCER_API UPlayerStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPlayerStatsComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	bool ModifyStat(EStatisticsType StatType, int Delta);

	float GetStat(EStatisticsType StatType) const;

private:
	// Unable to use Map due to UE Replicated restrictions
	UPROPERTY(Replicated)
	TArray<float> M_NormalizedStats{
		0, 0, 0, 0, 0, 0, 0
	};

	UPROPERTY(Replicated)
	TArray<float> M_Stats{
		0, 0, 0, 0, 0, 0, 0
	};

	TMap<EStatisticsType, float> M_Stats_Multipliers{
		{KILLS, 1}, {DEATHS, -1}, {ASSISTS, 0.5}, {GOALS_SCORED, 2}, {GOALS_SAVED, 1}, {SCORE, 1}, {READY_STATUS, 0}
	};
	TMap<EStatisticsType, double> M_Stats_NormalizationMultipliers{
		{KILLS, 1}, {DEATHS, 1}, {ASSISTS, 1}, {GOALS_SCORED, 1.0 / 50}, {GOALS_SAVED, 1}, {SCORE, 1}, {READY_STATUS, 1}
	};
};
