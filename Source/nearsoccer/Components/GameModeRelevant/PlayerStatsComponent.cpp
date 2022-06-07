// Nearsoccer 2022


#include "PlayerStatsComponent.h"

#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UPlayerStatsComponent::UPlayerStatsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPlayerStatsComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPlayerStatsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPlayerStatsComponent, M_NormalizedStats);
	DOREPLIFETIME(UPlayerStatsComponent, M_Stats);
}


// Called every frame
void UPlayerStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UPlayerStatsComponent::ModifyStat(EStatisticsType StatType, int Delta)
{
	if (StatType >= 0 && StatType < M_Stats_Multipliers.Num())
	{
		M_Stats[StatType] += Delta;
		auto OldVal = M_NormalizedStats[StatType];
		M_NormalizedStats[StatType] = static_cast<int>(M_Stats[StatType] * M_Stats_NormalizationMultipliers[StatType]);

		int RealDelta = M_NormalizedStats[StatType] - OldVal;
		UE_LOG(LogTemp, Warning, TEXT("MODIFIYING STATS REAL %f NORMALIZED %f MULTIPLIER %f"), M_Stats[StatType],
		       M_NormalizedStats[StatType], M_Stats_NormalizationMultipliers[StatType]);
		if (RealDelta != 0)
		{
			if (SCORE != StatType)
			{
				M_NormalizedStats[SCORE] += RealDelta * M_Stats_Multipliers[StatType];
			}

			return true;
		}
	}
	return false;
}

float UPlayerStatsComponent::GetStat(EStatisticsType StatType) const
{
	if (!(StatType >= 0 && StatType < M_Stats_Multipliers.Num()))
	{
		UE_LOG(LogTemp, Warning, TEXT("StatType should stay between 0 and %d"), SCORE);
		return -1;
	}
	return M_NormalizedStats[StatType];
}
