// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/NearsoccerCore.h"
#include "TeamComponent.generated.h"


class ANearsoccerPlayerState;
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NEARSOCCER_API UTeamComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTeamComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	// Unable to use constructor, because creating object via CreateDefaultSubobject().
	void Init(ETeam Type, FLinearColor TColor);
	void AddMember(APlayerController* Player);
	void RemoveMember(APlayerController* Player);
	int GetScore() const;
	int GetPlayersCount() const;
	int GetTeamId() const;
	ETeam GetTeamType() const;
	FLinearColor GetColor() const;
	void ChangeScore(int DeltaScore);
	TArray<APlayerState*> GetPlayers();

private:
	bool IsInitialized = false;
	
	UPROPERTY(Replicated)
	ETeam TeamType = ETeam::ENone;

	UPROPERTY(Replicated)
	FLinearColor TeamColor = FLinearColor(0.0f, 0.0f, 0.0f, 1);
	
	// Unable to use Map due to UE Replicated restrictions
	// TODO: implement array rebuild
	UPROPERTY(Replicated)
	TArray<APlayerState*> PlayersArray;
	TMap<int, int> PlayersIndexes;

	UPROPERTY(Replicated)
	int Score = 0;
	
	UPROPERTY(Replicated)
	int PlayersCount = 0;
};
