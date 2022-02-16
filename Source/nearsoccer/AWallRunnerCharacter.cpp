// Free for use --- Nearsoccer 2022


#include "AWallRunnerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AAWallRunnerCharacter::AAWallRunnerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAWallRunnerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (bIsDoubleJumping) {
		JumpMaxCount = 2;
	}
	GetCharacterMovement()->AirControl = 0.3f;
}

// Called every frame
void AAWallRunnerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Handles moving forward/backward
void AAWallRunnerCharacter::MoveForward(float Val)
{
	if (Val != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Val);
	}
}

// Handles strafing left/right movement
void AAWallRunnerCharacter::MoveRight(float Val)
{
	if (Val != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Val);
	}
}

// Called to bind functionality to input
void AAWallRunnerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Movement while on standing on ground bindings
	PlayerInputComponent->BindAxis("MoveForward", this, &AAWallRunnerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AAWallRunnerCharacter::MoveRight);

	// Mouse bindings
	PlayerInputComponent->BindAxis("Turn", this, &AAWallRunnerCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AAWallRunnerCharacter::AddControllerPitchInput);

	// Jump binding
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AAWallRunnerCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AAWallRunnerCharacter::StopJumping);
}

