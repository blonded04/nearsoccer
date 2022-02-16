// Free for use --- Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AWallRunnerCharacter.generated.h"

UCLASS(config=Game)
class NEARSOCCER_API AAWallRunnerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAWallRunnerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Double-jump by default
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	bool bIsDoubleJumping;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing left/right movement */
	void MoveRight(float Val);

public:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
