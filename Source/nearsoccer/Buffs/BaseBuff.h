// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "WallRunnerCharacter.h"
#include "UObject/Object.h"
#include "Components/Image.h"
#include "BaseBuff.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class NEARSOCCER_API UBaseBuff : public UObject
{
	GENERATED_BODY()

	FTimerHandle BuffHandle;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="BuffParams")
	FString BuffName{"Based"};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BuffParams")
	float BuffTime{5.0f};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BuffParams")
	FVector BuffDistanceFromSource{0.0f};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BuffParams")
	float BuffRadius{500.0f};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BuffParams")
	bool bIsResets{true};

	UPROPERTY(BlueprintReadOnly, Category="BuffParams")
	ACharacter* BuffCharacter{nullptr};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BuffParams")
	UTexture* BuffImage;
	
	UPROPERTY(EditDefaultsOnly, Category="FX Params")
	float FXRadius{50.0f};

	UPROPERTY(EditDefaultsOnly, Category="FX Params")
	FLinearColor FXColor;
	
	UPROPERTY(EditDefaultsOnly, Category="FX Params")
	bool bIsSpawnCircleEffect{false};
	
	UPROPERTY(EditDefaultsOnly, Category="FX Params")
	bool bIsLeftRight;

	UPROPERTY(BlueprintReadOnly, Category="BuffOwner")
	ATestPlayerState* BuffInstigator;

public:
	UBaseBuff();

	FString GetName() const;

	float GetRadius() const;

	UFUNCTION(BlueprintNativeEvent)
	void Activate(ACharacter* Character);

	UFUNCTION(BlueprintNativeEvent)
	void Deactivate();

	UFUNCTION(BlueprintCallable)
	class UBuffComponent* GetBuffComponent();

	virtual void ResetTimer();

	void SetDistance(FVector Distance);

	void SetBuffInstigator(ATestPlayerState* PlayerState);

	bool IsResets() const { return bIsResets; }

	UTexture* GetTexture() const { return BuffImage; }

	FLinearColor GetFXColor() const { return FXColor; }

	float GetFXRadius() const { return FXRadius; }

	bool IsLeftRight() const { return bIsLeftRight; }

	bool IsSpawnCircleEffect() const { return bIsSpawnCircleEffect; }

	float GetTime() const { return BuffTime; }
};
