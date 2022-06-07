// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "BuffInfo.h"
#include "TestPlayerState.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEARSOCCER_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBuffComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY()
	TArray<class UBaseBuff*> BuffsArray;
	
	UPROPERTY(Replicated)
	TArray<FBuffInfo> BuffsInfoArray;
	
	UPROPERTY(Replicated)
	bool bIsHasteActivated{false};

	UPROPERTY(Replicated)
	bool bIsBounceActivated{false};
	
	UPROPERTY(Replicated)
	bool bIsShieldActivated{false};
	
	UPROPERTY(Replicated)
	bool bIsGunPowerActivated{false};
	
	UPROPERTY(Replicated)
	bool bIsFlashed{false};
	
	UPROPERTY(Replicated)
	bool bIsSlowed{false};
	
	UPROPERTY(Replicated)
	bool bIsFreeze{false};
	
	UPROPERTY(Replicated)
	float MoveSpeedMultiplier{1.0f};

public:
	void AddBuff(TSubclassOf<UBaseBuff> BuffClass, FVector DistanceFromSource, ATestPlayerState* BuffInstigator=nullptr);

	void RemoveBuff(UBaseBuff* Buff);

	UFUNCTION(BlueprintCallable)
	bool CheckIsHasteActivated() const { return bIsHasteActivated; }

	UFUNCTION(BlueprintCallable)
	bool CheckIsBounceActivated() const { return bIsBounceActivated; }

	UFUNCTION(BlueprintCallable)
	bool CheckIsShieldActivated() const { return bIsShieldActivated; }

	UFUNCTION(BlueprintCallable)
	bool CheckIsGunPowerActivated() const { return bIsGunPowerActivated; }

	UFUNCTION(BlueprintCallable)
	bool CheckIsFlashed() const { return bIsFlashed; }

	UFUNCTION(BlueprintCallable)
	bool CheckIsSlowed() const { return bIsSlowed; }

	UFUNCTION(BlueprintCallable)
	bool CheckIsFreeze() const { return bIsFreeze; }

	float GetMoveSpeedMultiplier() const { return MoveSpeedMultiplier; }
	
	FString GetBuffsStatus() const;

	UFUNCTION(BlueprintCallable)
	void SetIsHasteActivated(float HasteMulti, bool Value);

	UFUNCTION(BlueprintCallable)
	void SetJumpVelocity(float BounceMulti, bool IsActivating);

	UFUNCTION(BlueprintCallable)
	void SetIsShieldActivated(bool Value);
	
	UFUNCTION(BlueprintCallable)
	void SetIsGunPowerActivated(bool Value);
	
	UFUNCTION(BlueprintCallable)
	void SetIsFlashed(bool Value);
	
	UFUNCTION(BlueprintCallable)
	void SetIsSlowed(float SlowMulti, bool Value);
	
	UFUNCTION(BlueprintCallable)
	void SetIsFreeze(bool Value);

	const TArray<FBuffInfo>& GetBuffsInfo() const { return BuffsInfoArray; }
	
protected:

	UFUNCTION(Server, Reliable)
	void ServerSetIsHasteActivated(float HasteMulti, bool Value);
	
	UFUNCTION(Server, Reliable)
	void ServerSetJumpVelocity(float BounceMulti, bool IsActivating);
	
	UFUNCTION(Server, Reliable)
	void ServerSetIsShieldActivated(bool Value);
	
	UFUNCTION(Server, Reliable)
	void ServerSetIsGunPowerActivated(bool Value);
	
	UFUNCTION(Server, Reliable)
	void ServerSetIsFlashed(bool Value);
	
	UFUNCTION(Server, Reliable)
	void ServerSetIsSlowed(float SlowMulti, bool Value);
	
	UFUNCTION(Server, Reliable)
	void ServerSetIsFreeze(bool Value);
	
};
