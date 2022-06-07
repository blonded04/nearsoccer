// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "NearsoccerCore.h"
#include "TestPlayerState.h"
#include "Logics/Gamemode/TestGameMode.h"
#include "UObject/Object.h"
#include "BuffsFunctionLib.generated.h"

/**
 * 
 */
UCLASS()
class NEARSOCCER_API UBuffsFunctionLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static void ApplyBuffAtLocation(TSubclassOf<class UBaseBuff> BuffClass, FVector Location, 
	ETeam InstigatorTeam, bool bIsOnInstigatorTeam, ATestGameMode* GameMode, USoundBase* SoundToSpawn);
	
	UFUNCTION(BlueprintCallable)
    static void GrenadeExplosionAtLocation(class ANearGrenade* Grenade, ETeam InstigatorTeam, bool bIsOnInstigatorTeam,
    bool bWhenVisible, ATestPlayerState* GrenadeInstigator=nullptr);

	static bool DoLineTrace(AActor* StartActor, class AWallRunnerCharacter* Target, UWorld* World, bool bWhenVisible);
	
	static bool CheckIsVisible(AActor* Grenade, AWallRunnerCharacter* Target, UWorld* World);
};
