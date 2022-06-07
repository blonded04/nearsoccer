// Nearsoccer 2022


#include "WallRunnerCharacter.h"

#include <limits>

#include "EngineUtils.h"
#include "Actors/Weapons/CombatWeapon.h"
#include "Actors/PlayerInteractions/Trampoline.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/Abilities/DashComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "NiagaraComponent.h"
#include "TestGameInstance.h"
#include "Actors/ObjectsWithPhysics/NearsoccerSoccerBall.h"
#include "TimerManager.h"
#include "TestGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Core/NearsoccerCore.h"
#include "TestGameState.h"
#include "TestPlayerState.h"
#include "WallRunnerController.h"

// ==================SETTING_UP==================

// Sets default values
AWallRunnerCharacter::AWallRunnerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(40.0f, 86.0f);

	// Attaching FPSCamera to capsule component
	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(
		TEXT("FirstPersonCamera"));
	check(FPSCameraComponent != nullptr);
	FPSCameraComponent->SetupAttachment(
		CastChecked<USceneComponent, UCapsuleComponent>(GetCapsuleComponent()));

	// Move camera up
	FPSCameraComponent->SetRelativeLocation(
		FVector(0.0f, 0.0f, 20.0f + BaseEyeHeight));

	// Allow camera rotation
	FPSCameraComponent->bUsePawnControlRotation = true;

	// Creating first person mesh
	FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(
		TEXT("FirstPersonMesh"));
	check(FPSMesh != nullptr);
	FPSMesh->SetOnlyOwnerSee(true);
	FPSMesh->SetupAttachment(FPSCameraComponent);
	FPSMesh->bCastDynamicShadow = false;
	FPSMesh->CastShadow = false;

	// Creating third person gun mesh
	TPSGunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(
		TEXT("ThirdPersonGunMesh"));
	check(TPSGunMesh != nullptr);
	TPSGunMesh->SetOwnerNoSee(true);
	TPSGunMesh->SetupAttachment(GetMesh(), TEXT("GunSocket"));
	TPSGunMesh->bCastDynamicShadow = true;
	TPSGunMesh->CastShadow = true;

	// Creating FX mesh
	FXMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FXMesh"));
	check(FXMesh != nullptr);
	FXMesh->SetHiddenInGame(true);
	FXMesh->SetupAttachment(GetMesh());

	// Creating indicators niagara components
	IndicatorPrimaryFire = CreateDefaultSubobject<UNiagaraComponent>(TEXT("IndicatorPrimaryFire"));
	IndicatorSecondaryFire = CreateDefaultSubobject<UNiagaraComponent>(TEXT("IndicatorSecondaryFire"));
	IndicatorPrimaryFire->SetupAttachment(FPSMesh, TEXT("M1_Indicator_Socket"));
	IndicatorSecondaryFire->SetupAttachment(FPSMesh, TEXT("M2_Indicator_Socket"));

	GrenadeSpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("GrenadeSpawnLocation"));
	GrenadeSpawnLocation->SetupAttachment(FPSMesh, TEXT("GrenadeSpawn"));

	// Hide own third-person mesh
	GetMesh()->SetOwnerNoSee(true);

	// Change AirControl
	GetCharacterMovement()->AirControl = AirControl;

	// Sets Max Walk Speed (only HASTE buff can override that value)
	GetCharacterMovement()->MaxWalkSpeed = DefaultMaxMoveSpeed;

	GetCharacterMovement()->SetIsReplicated(true);

	// Create WallRunning component
	WallRunComponent = CreateDefaultSubobject<UWallRunComponent>(TEXT("WallRunComponent"));
	check(WallRunComponent != nullptr);

	// Create Dash component
	DashComponent = CreateDefaultSubobject<UDashComponent>(TEXT("DashComponent"));
	check(DashComponent != nullptr);

	// Create Inventory component
	InventoryComp = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");
	check(InventoryComp != nullptr);

	// Create Buff component
	BuffComp = CreateDefaultSubobject<UBuffComponent>("BuffComponent");
	check(BuffComp != nullptr);

	// ServerSetTeamId(FLinearColor(0.0f, 0.0f, 0.0f, 1));
}

void AWallRunnerCharacter::SetGameTimerValue_Implementation(int TimeLeft)
{
	if (GameInfoWidget)
	{
		GameInfoWidget->ChangeTimeText(TimeLeft);
	}
}

// Called when the game starts or when spawned
void AWallRunnerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Allow double-jumping
	if (bDoubleJumping)
	{
		JumpMaxCount = 2;
	}

	// Spawn weapon
	if (HasAuthority() && WeaponClass != nullptr)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Owner = this;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Weapon = GetWorld()->SpawnActor<ACombatWeapon>(WeaponClass,
		                                               FPSCameraComponent->GetComponentTransform(),
		                                               SpawnInfo);
	}
	if (Weapon == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("WallRunnerCharacter: %p was spawned without a weapon"), this)
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("WallRunnerCharacter: %p was spawned with a weapon %p"), this, Weapon);
	}

	// Draw HUD
	DrawHUDWidgets();

	// Enable bumping off high-speed objects
	OnActorHit.AddDynamic(this, &AWallRunnerCharacter::OnHit);
}

void AWallRunnerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	DestroyHUDWidgets();
}

void AWallRunnerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWallRunnerCharacter, Weapon);
	DOREPLIFETIME(AWallRunnerCharacter, PlayerTeamId);
	DOREPLIFETIME(AWallRunnerCharacter, bIsInLava);
	DOREPLIFETIME(AWallRunnerCharacter, bIsDead);
	DOREPLIFETIME(AWallRunnerCharacter, FlashOpacity);
	DOREPLIFETIME(AWallRunnerCharacter, CurrentMaxMoveSpeed);
	DOREPLIFETIME(AWallRunnerCharacter, FXSpawned);
}

// Called to bind functionality to input
void AWallRunnerCharacter::SetupPlayerInputComponent(
	UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Move bindings
	PlayerInputComponent->BindAxis("MoveForward", this,
	                               &AWallRunnerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this,
	                               &AWallRunnerCharacter::MoveRight);

	// Look bindings
	PlayerInputComponent->BindAxis("Turn", this,
	                               &AWallRunnerCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this,
	                               &AWallRunnerCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("TurnRate", this,
	                               &AWallRunnerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this,
	                               &AWallRunnerCharacter::LookUpAtRate);

	// Jump bindings
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this,
	                                 &AWallRunnerCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this,
	                                 &AWallRunnerCharacter::StopJumping);

	// Wall Ride bindings
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this,
	                                 &AWallRunnerCharacter::BeginRiding);
	PlayerInputComponent->BindAction("Jump", IE_Released, this,
	                                 &AWallRunnerCharacter::StopRiding);

	// Dash bindings
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AWallRunnerCharacter::DashInput);

	// Crouch bindings
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AWallRunnerCharacter::BeginCrouching);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AWallRunnerCharacter::StopCrouching);

	// Inventory: PowerUp activate bind
	PlayerInputComponent->BindAction("ActivatePowerUp", IE_Pressed, this,
	                                 &AWallRunnerCharacter::ActivatePowerUp);

	// Inventory: Grenade throw bind
	PlayerInputComponent->BindAction("ThrowGrenade", IE_Pressed, this,
	                                 &AWallRunnerCharacter::ThrowGrenade);

	// Only for test: Death activate bind
	PlayerInputComponent->BindAction("Die", IE_Pressed, this,
	                                 &AWallRunnerCharacter::GetHurtBind);
	// Swap team binding
	// PlayerInputComponent->BindAction("ChangeTeam", IE_Pressed, this, &AWallRunnerCharacter::ChangeTeamServer);
	PlayerInputComponent->BindAction("ChangeTeam", IE_Pressed, this, &AWallRunnerCharacter::ToggleTeamSelect);

	//PlayerInputComponent->BindAction("SpawnNiagara", IE_Pressed, this, &AWallRunnerCharacter::SpawnFX);

	// Scoreboard
	PlayerInputComponent->BindAction("Tab", IE_Pressed, this, &AWallRunnerCharacter::OpenScoreboard);
	PlayerInputComponent->BindAction("Tab", IE_Released, this, &AWallRunnerCharacter::DestroyScoreboard);

	// Weapon bindings
	PlayerInputComponent->BindAction("PrimaryFire", IE_Pressed, this,
	                                 &AWallRunnerCharacter::WeaponPrimaryFireBegin);
	PlayerInputComponent->BindAction("PrimaryFire", IE_Released, this,
	                                 &AWallRunnerCharacter::WeaponPrimaryFireEnd);
	PlayerInputComponent->BindAction("SecondaryFire", IE_Pressed, this,
	                                 &AWallRunnerCharacter::WeaponSecondaryFireBegin);
	PlayerInputComponent->BindAction("SecondaryFire", IE_Released, this,
	                                 &AWallRunnerCharacter::WeaponSecondaryFireEnd);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AWallRunnerCharacter::WeaponReload);


	PlayerInputComponent->BindAction("Teleport", IE_Pressed, this, &AWallRunnerCharacter::PlayerReady);
	PlayerInputComponent->BindAction("Escape", IE_Pressed, this, &AWallRunnerCharacter::ToggleEscape);
}

// Called every frame
void AWallRunnerCharacter::Tick(float DeltaTime)
{
	VOID_IS_VALID(this);
	Super::Tick(DeltaTime);

	FlashOpacity -= DeltaTime / 2;
	if (FlashOpacity < 0)
	{
		FlashOpacity = 0.0f;
	}
}

// ===================MOVEMENT===================

// Bounce after fall implementation
void AWallRunnerCharacter::Bounce_Implementation(AWallRunnerCharacter* Player, const FVector& Vec)
{
	VOID_IS_VALID(Player);
	if (!GetCharacterMovement()->IsCrouching())
	{
		Player->LaunchCharacter(Vec, true, true);
	}
}

// Add bouncing off the floor after high speed fall
void AWallRunnerCharacter::Landed(const FHitResult& Hit)
{
	VOID_IS_VALID(this);

	auto Vel = GetVelocity();

	Super::Landed(Hit);

	UE_LOG(LogTemp, Warning, TEXT("Landed, V: %f"), Vel.Size());
	if (Cast<ATrampoline>(Hit.GetActor()))
	{
		return;
	}
	if (Vel.Size() >= 900.0f)
	{
		float Len = Vel.Size() / 2.5f;
		Vel.Normalize();
		Bounce(this, FMath::GetReflectionVector(Vel, Hit.Normal) * std::min(910.0f, Len));
		UE_LOG(LogTemp, Warning, TEXT("Bouncing, V: %f"), std::min(910.0f, Len));
	}
}

// Process prop hit
void AWallRunnerCharacter::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse,
                                 const FHitResult& Hit)
{
	VOID_IS_VALID(SelfActor);
	VOID_IS_VALID(OtherActor);

	auto Plr = Cast<AWallRunnerCharacter>(SelfActor);
	check(Plr != nullptr);

	auto Ball = Cast<ANearsoccerSoccerBall>(OtherActor);
	if (Ball != nullptr)
	{
		Ball->SetLastPlayerBlocked(Cast<AWallRunnerController>(this->GetController()));
	}
}

// Handles input for moving forward and backward.
void AWallRunnerCharacter::MoveForward(float Val)
{
	VOID_IS_VALID(this);
	VOID_CHECK(bIsDead || BuffComp->CheckIsFreeze(), false);

	UpdateMaxSpeed();
	if (!WallRunComponent->IsOnWall())
	{
		AddMovementInput(GetActorForwardVector(), Val);
	}
}

// Handles input for moving right and left.
void AWallRunnerCharacter::MoveRight(float Val)
{
	VOID_IS_VALID(this);
	VOID_CHECK(bIsDead || BuffComp->CheckIsFreeze(), false);

	UpdateMaxSpeed();
	if (!WallRunComponent->IsOnWall())
	{
		AddMovementInput(GetActorRightVector(), Val);
	}
}

void AWallRunnerCharacter::LeaveServer()
{
	if (auto Instance = Cast<UTestGameInstance>(GetGameInstance()))
	{
		Instance->Leave();
	}
	// CallDestroy();
}

void AWallRunnerCharacter::WrapNameWithTeamTag(FString& Name, ETeam TeamType)
{
	FString tag = "<Blue>";
	if (TeamType == ETeam::ERed)
	{
		tag = "<Red>";
	}
	Name = tag + Name + "</>";
}

void AWallRunnerCharacter::ChangeName(const FString& Name)
{
	UE_LOG(LogTemp, Warning, TEXT("Change name local"));
	ChangeNameServer(Name);
}

// Updating MaxWalkSpeed
void AWallRunnerCharacter::UpdateMaxSpeed()
{
	VOID_IS_VALID(this);
	if (GetCharacterMovement()->MaxWalkSpeed != DefaultMaxMoveSpeed * BuffComp->GetMoveSpeedMultiplier())
	{
		ServerUpdateMaxSpeed();
		GetCharacterMovement()->MaxWalkSpeed = CurrentMaxMoveSpeed;
		UE_LOG(LogTemp, Warning, TEXT("New Move Speed: %f"), CurrentMaxMoveSpeed);
	}
}

void AWallRunnerCharacter::Jump()
{
	VOID_CHECK(bIsDead || BuffComp->CheckIsFreeze(), false);
	Super::Jump();
}

void AWallRunnerCharacter::AddControllerPitchInput(float Val)
{
	VOID_CHECK(bIsDead || BuffComp->CheckIsFreeze(), false);

	Super::AddControllerPitchInput(Val);
}

void AWallRunnerCharacter::AddControllerYawInput(float Val)
{
	VOID_CHECK(bIsDead || BuffComp->CheckIsFreeze(), false);

	Super::AddControllerYawInput(Val);
}

void AWallRunnerCharacter::BeginRiding()
{
	VOID_CHECK(bIsDead || BuffComp->CheckIsFreeze(), false);

	WallRunComponent->BeginRiding();
}

void AWallRunnerCharacter::StopRiding()
{
	WallRunComponent->StopRiding();
}

void AWallRunnerCharacter::DashInput()
{
	VOID_CHECK(bIsDead || BuffComp->CheckIsFreeze(), false);

	DashComponent->DashInput();
}

// Updating MaxWalkSpeed on server
void AWallRunnerCharacter::ServerUpdateMaxSpeed_Implementation()
{
	VOID_IS_VALID(this);
	CurrentMaxMoveSpeed = DefaultMaxMoveSpeed * BuffComp->GetMoveSpeedMultiplier();
	GetCharacterMovement()->MaxWalkSpeed = CurrentMaxMoveSpeed;
}

// Starts crouching
void AWallRunnerCharacter::BeginCrouching()
{
	VOID_IS_VALID(this);
	VOID_IS_VALID(GetCharacterMovement());
	VOID_CHECK(bIsDead || BuffComp->CheckIsFreeze(), false);

	if (!GetCharacterMovement()->IsCrouching())
	{
		GetCharacterMovement()->bWantsToCrouch = true;
		GetCharacterMovement()->Crouch();
	}
}


// Stops crouching
void AWallRunnerCharacter::StopCrouching()
{
	VOID_IS_VALID(this);
	VOID_IS_VALID(GetCharacterMovement());
	VOID_CHECK(bIsDead || BuffComp->CheckIsFreeze(), false);

	if (GetCharacterMovement()->IsCrouching())
	{
		GetCharacterMovement()->bWantsToCrouch = false;
		GetCharacterMovement()->UnCrouch();
	}
}

void AWallRunnerCharacter::TurnAtRate(float Rate)
{
	VOID_IS_VALID(this);
	VOID_CHECK(bIsDead || BuffComp->CheckIsFreeze(), false);

	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AWallRunnerCharacter::LookUpAtRate(float Rate)
{
	VOID_IS_VALID(this);
	VOID_CHECK(bIsDead || BuffComp->CheckIsFreeze(), false);
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

// ====================WIDGETS===================

// Draw all the HUD
void AWallRunnerCharacter::DrawHUDWidgets()
{
	VOID_IS_VALID(this);
	DestroyHUDWidgets();
	// Draw HUD
	if (IsLocallyControlled())
	{
		UE_LOG(LogTemp, Warning, TEXT("Drawing"));
		APlayerController* FPC = GetController<APlayerController>();
		check(FPC != nullptr);
		if (SpeedClass)
		{
			SpeedWidget = CreateWidget<USpeedWidget>(FPC, SpeedClass);
			check(SpeedWidget != nullptr);
			SpeedWidget->AddToViewport(1);
			UE_LOG(LogTemp, Warning, TEXT("HUD: Speed info drawn"));
		}
		if (CooldownClass)
		{
			CooldownWidget = CreateWidget<UCooldownWidget>(FPC, CooldownClass);
			check(CooldownWidget != nullptr);
			CooldownWidget->AddToViewport(1);
			UE_LOG(LogTemp, Warning, TEXT("HUD: Cooldown info drawn"));
		}
		if (FlashScreenClass)
		{
			FlashScreen = CreateWidget<UFlashScreen>(FPC, FlashScreenClass);
			check(FlashScreen != nullptr);
			FlashScreen->AddToViewport(500);
			UE_LOG(LogTemp, Warning, TEXT("HUD: Buff info drawn"));
		}
		if (BuffWidgetClass)
		{
			BuffWidget = CreateWidget<UBuffEffectsWidget>(FPC, BuffWidgetClass);
			check(BuffWidget != nullptr);
			BuffWidget->AddToViewport(1);
			UE_LOG(LogTemp, Warning, TEXT("HUD: Buff info drawn"));
		}
		if (HUDClass)
		{
			HUDWidget = CreateWidget<UUserWidget>(FPC, HUDClass);
			check(HUDWidget != nullptr);
			HUDWidget->AddToViewport(1);
			UE_LOG(LogTemp, Warning, TEXT("HUD: Buff info drawn"));
		}
		if (GameInfoClass)
		{
			GameInfoWidget = CreateWidget<UGameInfoWidget>(FPC, GameInfoClass);
			check(GameInfoWidget != nullptr);
			GameInfoWidget->AddToViewport(2);
			UE_LOG(LogTemp, Warning, TEXT("HUD: Game info drawn"));
			auto GS = Cast<ATestGameState>(GetWorld()->GetGameState());
			if (!GS->IsGameRunning())
				GameInfoWidget->SetInfoMessage("Press \"P\" to change your team\nPress \"M\" when you are ready");
		}
	}
}

void AWallRunnerCharacter::ChangeStencil_Implementation(ACharacter* OtherCharacter, int Stencil)
{
	VOID_IS_VALID(OtherCharacter);
	UE_LOG(LogTemp, Warning, TEXT("Changing Stencil %d"), OtherCharacter != nullptr);
	OtherCharacter->GetMesh()->SetCustomDepthStencilValue(Stencil);
	Cast<USkeletalMeshComponent>(OtherCharacter->GetMesh()->GetChildComponent(0))->SetCustomDepthStencilValue(Stencil);
}

// Destroy all the HUD
void AWallRunnerCharacter::DestroyHUDWidgets(bool bDestroyGameInfo)
{
	VOID_IS_VALID(this);

	UE_LOG(LogTemp, Warning, TEXT("HUD: Destroying entire HUD..."));
	if (ScoreboardWidget)
	{
		DestroyScoreboard();
	}
	if (EscapeWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("HUD: Game info widget destroyed"));
		EscapeWidget->RemoveFromParent();
		EscapeWidget = nullptr;
	}
	if (GameInfoWidget && bDestroyGameInfo)
	{
		UE_LOG(LogTemp, Warning, TEXT("HUD: Game info widget destroyed"));
		GameInfoWidget->RemoveFromParent();
		GameInfoWidget = nullptr;
	}
	if (SpeedWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("HUD: Speed widget destroyed"));
		SpeedWidget->RemoveFromParent();
		SpeedWidget = nullptr;
	}
	if (CooldownWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("HUD: Cooldown widget destroyed"));
		CooldownWidget->RemoveFromParent();
		CooldownWidget = nullptr;
	}
	if (FlashScreen)
	{
		UE_LOG(LogTemp, Warning, TEXT("HUD: Buff widget destroyed"));
		FlashScreen->RemoveFromParent();
		FlashScreen = nullptr;
	}
	if (BuffWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("HUD: Buff widget destroyed"));
		BuffWidget->RemoveFromParent();
		BuffWidget = nullptr;
	}
	if (HUDWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("HUD: HUD widget destroyed"));
		HUDWidget->RemoveFromParent();
		HUDWidget = nullptr;
	}
}

void AWallRunnerCharacter::ChangeScoreUI_Implementation(ETeam ScoredTeam, int NewScore)
{
	VOID_IS_VALID(GameInfoWidget);
	GameInfoWidget->ChangeScoreText(ScoredTeam, NewScore);
}

void AWallRunnerCharacter::RebuildScoreboard()
{
	if (ScoreboardWidget)
	{
		ScoreboardWidget->Update();
	}
}

void AWallRunnerCharacter::UpdateKillFeed(const FString& Killer, const FString& Killed, EDeathType DeathType,
                                          bool bWasKilled)
{
	if (GameInfoWidget)
	{
		GameInfoWidget->PushToKillFeed(Killer, Killed, DeathType, bWasKilled);
	}
}

void AWallRunnerCharacter::SetWinner(EWinnerType Winner)
{
	if (!GameInfoWidget)
	{
		if (GameInfoClass)
		{
			auto FPC = GetController<APlayerController>(); 
			GameInfoWidget = CreateWidget<UGameInfoWidget>(FPC, GameInfoClass);
			check(GameInfoWidget != nullptr);
			GameInfoWidget->AddToViewport(2);
			FPC->SetInputMode(FInputModeGameAndUI());
		}
	}
	
	GameInfoWidget->SetWinner(Winner);
	DestroyHUDWidgets(false);
	Cast<ATestGameState>(GetWorld()->GetGameState())->PlayWinningVideo(Winner);
}

const TArray<FBuffInfo>& AWallRunnerCharacter::GetBuffsInfoArray()
{
	static TArray<FBuffInfo> DefaultReturn;
	VALUE_IS_VALID(this, DefaultReturn);
	VALUE_IS_VALID(BuffComp, DefaultReturn);

	return BuffComp->GetBuffsInfo();
}

FBuffInfo AWallRunnerCharacter::GetPowerUpInfo() const
{
	FBuffInfo DefaultReturn;
	VALUE_IS_VALID(this, DefaultReturn);
	VALUE_IS_VALID(InventoryComp, DefaultReturn);

	return InventoryComp->GetPowerUpInfo();
}

FBuffInfo AWallRunnerCharacter::GetGrenadeInfo() const
{
	FBuffInfo DefaultReturn;
	VALUE_IS_VALID(this, DefaultReturn);
	VALUE_IS_VALID(InventoryComp, DefaultReturn);

	return InventoryComp->GetGrenadeInfo();
}

void AWallRunnerCharacter::OpenScoreboard()
{
	VOID_IS_VALID(this);
	VOID_CHECK(bIsDead, false);
	UE_LOG(LogTemp, Warning, TEXT("TAB pressed"));
	ScoreboardWidget = CreateWidget<UScoreboard>(Cast<APlayerController>(Controller), ScoreboardClass);
	ScoreboardWidget->AddToViewport(10);
}

void AWallRunnerCharacter::DestroyScoreboard()
{
	VOID_IS_VALID(ScoreboardWidget);
	ScoreboardWidget->RemoveFromParent();
	ScoreboardWidget = nullptr;
}

void AWallRunnerCharacter::ToggleEscape()
{
	auto FPC = GetController<APlayerController>();
	if (EscapeWidget)
	{
		EscapeWidget->RemoveFromParent();
		EscapeWidget = nullptr;
		FPC->SetShowMouseCursor(false);
		FPC->SetInputMode(FInputModeGameOnly());
	}
	else
	{
		if (EscapeClass)
		{
			EscapeWidget = CreateWidget<UEscapeWidget>(FPC, EscapeClass);
			check(EscapeWidget != nullptr);
			EscapeWidget->AddToViewport(100);
			FPC->SetShowMouseCursor(true);
			FPC->SetInputMode(FInputModeUIOnly());
		}
	}
}

void AWallRunnerCharacter::ToggleTeamSelect()
{
	auto GS = Cast<ATestGameState>(GetWorld()->GetGameState());
	if (GS->IsGameRunning())
		return;
	if (IsReady)
	{
		if (GameInfoWidget)
		{
			GameInfoWidget->SetErrorMessage(
				"Unable to change the team, while ready. Press \"M\" to change ready status.");
		}
		return;
	}
	auto FPC = GetController<APlayerController>();
	if (TeamSelectWidget)
	{
		TeamSelectWidget->RemoveFromParent();
		TeamSelectWidget = nullptr;
		FPC->SetShowMouseCursor(false);
		FPC->SetInputMode(FInputModeGameOnly());
	}
	else
	{
		if (TeamSelectClass)
		{
			TeamSelectWidget = CreateWidget<UTeamSelectWidget>(FPC, TeamSelectClass);
			check(TeamSelectWidget != nullptr);
			TeamSelectWidget->AddToViewport(100);
			FPC->SetShowMouseCursor(true);
			FPC->SetInputMode(FInputModeUIOnly());
		}
	}
}

// Testing info for Inventory and Buff information
FString AWallRunnerCharacter::GetHUDInfo() const
{
	VALUE_IS_VALID(this, "");
	VALUE_IS_VALID(InventoryComp, "");
	VALUE_IS_VALID(BuffComp, "");

	return "PowerUp: " + InventoryComp->GetPowerUpInfo().BuffName +
		"\nGrenade: " + InventoryComp->GetGrenadeInfo().BuffName +
		"\nActive effects:" + BuffComp->GetBuffsStatus();
}

// ==============BUFFS_AND_INVENTORY=============

void AWallRunnerCharacter::SetFlashOpacity(float Value)
{
	VOID_IS_VALID(this);
	UE_LOG(LogTemp, Warning, TEXT("Opacity: %f"), Value);
	ServerSetFlashOpacity(Value);
	FlashOpacity = Value;
	float SoundStartPoint = std::max(0.0f, 5.0f - 2 * Value);
	MultiSpawnFlashSound(SoundStartPoint);
}

void AWallRunnerCharacter::SpawnUISound_Implementation(USoundBase* Sound, float SoundStartPoint)
{
}

void AWallRunnerCharacter::MultiSpawnFlashSound_Implementation(float SoundStartPoint)
{
	if (IsLocallyControlled() && EarRingSound)
	{
		UE_LOG(LogTemp, Warning, TEXT("Sound Start"));
		UGameplayStatics::SpawnSound2D(GetWorld(), EarRingSound, 1.0f, 1, SoundStartPoint);
		SpawnUISound(EarRingSound, SoundStartPoint);
	}
}

void AWallRunnerCharacter::ServerSetFlashOpacity_Implementation(float Value)
{
	VOID_IS_VALID(this);
	FlashOpacity = Value;
}

void AWallRunnerCharacter::UpdateIsInLava(bool Value)
{
	VOID_IS_VALID(this);
	ServerUpdateIsInLava(Value);
}

void AWallRunnerCharacter::ServerUpdateIsInLava_Implementation(bool Value)
{
	VOID_IS_VALID(this);
	bIsInLava = Value;
	UE_LOG(LogTemp, Warning, TEXT("New Is In Lava Status: %d"), bIsInLava);
}

void AWallRunnerCharacter::SetImpulseVector(FVector Vec)
{
	VOID_IS_VALID(this);

	UE_LOG(LogTemp, Warning, TEXT("Vector Vec: %f, %f, %f"), Vec.X, Vec.Y, Vec.Z);
	WallRunComponent->StopRiding();
	FVector ToLaunch = Vec;
	if (ToLaunch.Z < 0)
	{
		ToLaunch.Z = 50.f;
		ToLaunch = ToLaunch.GetSafeNormal(0.001) * Vec.Size();
	}
	else
	{
		ToLaunch.Z = std::min(50.f, ToLaunch.Z);
	}
	UE_LOG(LogTemp, Warning, TEXT("Vector to Launch: %f, %f, %f"), ToLaunch.X, ToLaunch.Y, ToLaunch.Z);
	ServerSetImpulseVector(ToLaunch);
	LaunchCharacter(ToLaunch, true, true);
}

void AWallRunnerCharacter::ServerSetImpulseVector_Implementation(FVector Vec)
{
	VOID_IS_VALID(this);

	LaunchCharacter(Vec, true, true);
}

void AWallRunnerCharacter::SpawnFX(FBuffInfo Buff)
{
	VOID_IS_VALID(this);
	UE_LOG(LogTemp, Warning, TEXT("Spawning FX"));
	ServerSpawnFX(Buff);
}

void AWallRunnerCharacter::ServerSpawnFX_Implementation(FBuffInfo Buff)
{
	VOID_IS_VALID(this);

	UE_LOG(LogTemp, Warning, TEXT("Server Spawning FX"));
	MultiSpawnMeshEffect(Buff);

	if (Buff.bIsSpawnCircleEffect)
	{
		MultiSpawnCircleEffect(Buff);
	}
}

void AWallRunnerCharacter::MultiSpawnMeshEffect_Implementation(FBuffInfo Buff)
{
	VOID_IS_VALID(this);


	if (!IsLocallyControlled() && MeshEffect)
	{
		UE_LOG(LogTemp, Warning, TEXT("Multi Spawning Mesh FX"));
		SetUpMesh(Buff);
	}
}

void AWallRunnerCharacter::MultiSpawnCircleEffect_Implementation(FBuffInfo Buff)
{
	VOID_IS_VALID(this);
	if (!IsLocallyControlled() && CircleEffect)
	{
		UE_LOG(LogTemp, Warning, TEXT("Multi Spawning Circle FX"));
		FVector SpawnLoc;
		FRotator SpawnRot;
		FString SocketName = Buff.BuffName + "EffectSocket";
		if (!Buff.bIsLeftRight)
		{
			UMeshComponent* MeshToAttach = Buff.BuffName == "GunPower" ? TPSGunMesh : FXMesh;
			UNiagaraComponent* NewComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(CircleEffect, MeshToAttach,
				*SocketName, SpawnLoc, SpawnRot, EAttachLocation::KeepRelativeOffset, true);
			if (NewComponent)
			{
				UE_LOG(LogTemp, Warning, TEXT("Circle: Basic Spawned"));
				NewComponent->SetNiagaraVariableFloat("User.Radius", Buff.FXRadius);
				NewComponent->SetNiagaraVariableFloat("User.LifeTime", Buff.BuffTime);
				NewComponent->SetNiagaraVariableLinearColor("User.PartColor", Buff.FXColor);
			}
		}
		else
		{
			FString LeftSocketName = SocketName + "Left";
			UNiagaraComponent* NewLeftComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(CircleEffect, FXMesh,
				*LeftSocketName, SpawnLoc, SpawnRot, EAttachLocation::KeepRelativeOffset, true);
			if (NewLeftComponent)
			{
				UE_LOG(LogTemp, Warning, TEXT("Circle: Left Spawned"));
				NewLeftComponent->SetNiagaraVariableFloat("User.Radius", Buff.FXRadius);
				NewLeftComponent->SetNiagaraVariableFloat("User.LifeTime", Buff.BuffTime);
				NewLeftComponent->SetNiagaraVariableLinearColor("User.PartColor", Buff.FXColor);
			}

			FString RightSocketName = SocketName + "Right";
			UNiagaraComponent* NewRightComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(CircleEffect, FXMesh,
				*RightSocketName, SpawnLoc, SpawnRot, EAttachLocation::KeepRelativeOffset, true);
			if (NewRightComponent)
			{
				UE_LOG(LogTemp, Warning, TEXT("Circle: Right Spawned"));
				NewRightComponent->SetNiagaraVariableFloat("User.Radius", Buff.FXRadius);
				NewRightComponent->SetNiagaraVariableFloat("User.LifeTime", Buff.BuffTime);
				NewRightComponent->SetNiagaraVariableLinearColor("User.PartColor", Buff.FXColor);
			}
		}
	}
}

void AWallRunnerCharacter::SetUpMesh_Implementation(FBuffInfo Buff)
{
}

// Activating PowerUps
void AWallRunnerCharacter::ActivatePowerUp()
{
	VOID_IS_VALID(this);
	VOID_CHECK(bIsDead || BuffComp->CheckIsFreeze(), false);

	if (!HasAuthority())
	{
		ServerActivatePowerUp();
	}
	else
	{
		InventoryComp->ActivatePowerUp();
	}
}

// Server activating PowerUps
void AWallRunnerCharacter::ServerActivatePowerUp_Implementation()
{
	VOID_IS_VALID(this);
	if (HasAuthority())
	{
		ActivatePowerUp();
	}
}

// Throwing grenades
void AWallRunnerCharacter::ThrowGrenade()
{
	VOID_IS_VALID(this);
	VOID_CHECK(bIsDead || BuffComp->CheckIsFreeze(), false);

	if (!HasAuthority())
	{
		ServerThrowGrenade();
	}
	else
	{
		const FRotator SpawnRotation = GetControlRotation();
		FVector SpawnLocation;
		if (GrenadeSpawnLocation != nullptr)
		{
			SpawnLocation = GrenadeSpawnLocation->GetComponentLocation();
		}
		else
		{
			SpawnLocation = GetActorLocation();
		}
		InventoryComp->ThrowGrenade(SpawnLocation, SpawnRotation);
	}
}

// Server throwing grenades
void AWallRunnerCharacter::ServerThrowGrenade_Implementation()
{
	VOID_IS_VALID(this);
	if (HasAuthority())
	{
		ThrowGrenade();
	}
}

// ====================DAMAGE====================

void AWallRunnerCharacter::GetHurtBind()
{
	VOID_IS_VALID(this);
	VOID_CHECK(bIsDead || BuffComp->CheckIsFreeze(), false);

	GetHurt(false);
}

// Getting fatal damage 
void AWallRunnerCharacter::GetHurt(bool bIsServerCalled, ATestPlayerState* Killer, EDeathType DeathType,
                                   ATestPlayerState* KilledPlayerState)
{
	VOID_IS_VALID(this);
	if (!Cast<ATestGameState>(GetWorld()->GetGameState())->IsGameRunning())
	{
		return;
	}
	if (!BuffComp->CheckIsShieldActivated() && (IsLocallyControlled() || bIsServerCalled))
	{
		if (!KilledPlayerState && IsLocallyControlled())
		{
			KilledPlayerState = GetController()->GetPlayerState<ATestPlayerState>();
		}

		bIsDead = true;
		DestroyHUDWidgets();

		ServerGetHurt(Killer, DeathType, KilledPlayerState);
	}
}

// Server getting fatal damage
void AWallRunnerCharacter::ServerGetHurt_Implementation(ATestPlayerState* Killer, EDeathType DeathType,
                                                        ATestPlayerState* KilledPlayerState)
{
	VOID_IS_VALID(this);

	check(KilledPlayerState != nullptr);

	if (DeathType == ELava)
	{
		Killer = KilledPlayerState->GetLastPushed();
	}

	if (Killer)
	{
		int Delta = (Killer->GetTeamType() != KilledPlayerState->GetTeamType())
			            ? 1
			            : -1;

		UE_LOG(LogTemp, Warning, TEXT("KILL: killed other %d"), Delta);
		Killer->ModifyStat(EStatisticsType::KILLS, 1);
	}

	FString KillerName = (Killer) ? Killer->GetPlayerName() : "";
	FString KilledName = GetPlayerState()->GetPlayerName();
	if (Killer)
	{
		WrapNameWithTeamTag(KillerName, Killer->GetTeamType());
	}
	WrapNameWithTeamTag(KilledName, KilledPlayerState->GetTeamType());

	auto GState = Cast<ATestGameState>(GetWorld()->GetGameState());
	GState->UpdateKillFeed(KillerName, KilledName, DeathType,
	                       !(Killer == nullptr || Killer == KilledPlayerState));
	GState->ServerOnPlayerDied(KilledPlayerState->GetTeamType());
	if (GState->GetCurrentGameState() == EGameState::EInProgress)
	{
		bIsDead = true;
		Die();
	}
}

// Dying (called on server)
void AWallRunnerCharacter::Die()
{
	VOID_IS_VALID(this);
	if (HasAuthority())
	{
		MultiDie();
		Cast<ATestPlayerState>(Controller->PlayerState)->ModifyStat(DEATHS, 1);

		AGameModeBase* GM = GetWorld()->GetAuthGameMode();
		float DestroyTimer = 5.0f;
		if (ATestGameMode* GameMode = Cast<ATestGameMode>(GM))
		{
			DestroyTimer = GameMode->GetRespawnTimer() + 5.125f;
			GameMode->Respawn(GetController());
		}
		GetWorld()->GetTimerManager().SetTimer(RespawnHandle, this,
		                                       &AWallRunnerCharacter::CallDestroy,
		                                       DestroyTimer, false);
	}
}

// Dying for all clients
void AWallRunnerCharacter::MultiDie_Implementation()
{
	VOID_IS_VALID(this);

	// Stop fire
	if (Weapon != nullptr)
	{
		Weapon->StopFire();
	}
	// Disable HUD
	DestroyHUDWidgets();
	// Disabling movement and input
	GetCharacterMovement()->DisableMovement();
	// Ragdoll
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName("PhysicsActor");
	GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	// Show own third-person mesh
	GetMesh()->SetOwnerNoSee(false);
	TPSGunMesh->SetOwnerNoSee(false);
	// Hide own first-person mesh
	FPSMesh->SetOwnerNoSee(true);
	// Hide indicators
	IndicatorPrimaryFire->SetOwnerNoSee(true);
	IndicatorSecondaryFire->SetOwnerNoSee(true);

	// Spawning Death Sound
	if (DeathSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), DeathSound, GetMesh()->GetComponentLocation());
	}
}

// Destroys actor on server
void AWallRunnerCharacter::CallDestroy()
{
	VOID_IS_VALID(this);
	if (HasAuthority())
	{
		Destroy();
	}
}


// ================TEAM_AND_COLOR================

void AWallRunnerCharacter::SetTeamId(int TeamId)
{
	VOID_IS_VALID(this);
	ServerSetTeamId(TeamId);
	if (GetNetMode() < NM_Client)
	{
		OnRep_PlayerTeamId();
	}
}

void AWallRunnerCharacter::ServerSetTeamId_Implementation(int TeamId)
{
	VOID_IS_VALID(this);
	if (HasAuthority())
	{
		PlayerTeamId = TeamId;
		//OnRep_PlayerColor();
	}
}


void AWallRunnerCharacter::OnRep_PlayerTeamId()
{
	VOID_IS_VALID(this);
	UMaterialInstanceDynamic* MeshDynMatInst = GetMesh()->CreateDynamicMaterialInstance(
		0, GetMesh()->GetMaterial(0));
	MeshDynMatInst->SetVectorParameterValue("BodyColor", GetPlayerColor());

	UMaterialInstanceDynamic* FXMeshDynMatInst = FXMesh->CreateDynamicMaterialInstance(
		0, FXMesh->GetMaterial(0));
	FXMeshDynMatInst->SetVectorParameterValue("BodyColor", GetPlayerColor());

	FLinearColor GunColor = GetPlayerColor();
	GunColor.R = GunColor.R >= 0.5f ? GunColor.R : GunColor.R / 3;
	GunColor.G = GunColor.G >= 0.5f ? GunColor.G : GunColor.G / 3;
	GunColor.B = GunColor.B >= 0.5f ? GunColor.B : GunColor.B / 3;
	UMaterialInstanceDynamic* FPSMeshDynMatInst = FPSMesh->CreateDynamicMaterialInstance(
		0, FPSMesh->GetMaterial(0));
	FPSMeshDynMatInst->SetVectorParameterValue("BodyColor", GunColor);

	FPSMesh->SetCustomDepthStencilValue(PlayerTeamId + 12);
}

void AWallRunnerCharacter::ChangeTeamServer_Implementation()
{
	VOID_IS_VALID(this);
	VOID_CHECK(bIsDead || BuffComp->CheckIsFreeze(), false);

	if (HasAuthority())
	{
		auto GS = Cast<ATestGameState>(GetWorld()->GetGameState());
		GS->ChangePlayerTeam(
			Cast<APlayerController>(Controller));
		auto PS = Cast<ATestPlayerState>(GetController()->PlayerState);
		SetTeamId(PS->GetTeamId());
		GS->MovePlayerToSpawnPoint(this, EStartType::HUB, PS->GetTeamType());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Trying to change team, but have no authority"));
	}
}

// ===========Weapon-related functions===========

void AWallRunnerCharacter::ChangeNameServer_Implementation(const FString& Name)
{
	UE_LOG(LogTemp, Warning, TEXT("Trying to change name"));
	auto GS = Cast<ATestGameState>(GetWorld()->GetGameState());
	GS->ChangePlayerName(GetPlayerState(), Name);
}

void AWallRunnerCharacter::WeaponPrimaryFireBegin()
{
	VOID_IS_VALID(this);
	VOID_CHECK(bIsDead || BuffComp->CheckIsFreeze(), false);

	if (Weapon != nullptr)
	{
		Weapon->PrimaryFireBegin();
	}
}

void AWallRunnerCharacter::WeaponPrimaryFireEnd()
{
	VOID_IS_VALID(this);
	VOID_CHECK(bIsDead || BuffComp->CheckIsFreeze(), false);

	if (Weapon != nullptr)
	{
		Weapon->PrimaryFireEnd();
	}
}

void AWallRunnerCharacter::WeaponSecondaryFireBegin()
{
	VOID_IS_VALID(this);
	VOID_CHECK(bIsDead || BuffComp->CheckIsFreeze(), false);

	if (Weapon != nullptr)
	{
		Weapon->SecondaryFireBegin();
	}
}

void AWallRunnerCharacter::WeaponSecondaryFireEnd()
{
	VOID_IS_VALID(this);
	VOID_CHECK(bIsDead || BuffComp->CheckIsFreeze(), false);

	if (Weapon != nullptr)
	{
		Weapon->SecondaryFireEnd();
	}
}

void AWallRunnerCharacter::WeaponReload()
{
	VOID_IS_VALID(this);
	VOID_CHECK(bIsDead || BuffComp->CheckIsFreeze(), false);

	if (Weapon != nullptr)
	{
		Weapon->Reload();
	}
}

void AWallRunnerCharacter::SetSpawnTimerValue_Implementation(int TimeLeft)
{
	UE_LOG(LogTemp, Warning, TEXT("HUD: Spawn timer widget TimeLeft %d"), TimeLeft);
	if (TimeLeft <= 0)
	{
		if (SpawnTimerWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("HUD: Spawn timer widget destroyed"));
			SpawnTimerWidget->RemoveFromParent();
			SpawnTimerWidget = nullptr;
		}
	}
	else
	{
		if (!SpawnTimerWidget)
		{
			SpawnTimerWidget = CreateWidget<USpawnTimerWidget>(GetController<APlayerController>(), SpawnTimerClass);
			check(SpawnTimerWidget != nullptr);
			SpawnTimerWidget->AddToViewport(1);
			UE_LOG(LogTemp, Warning, TEXT("HUD: SpawnTimer widget drawn"));
		}
		SpawnTimerWidget->SetTimeLeft(TimeLeft);
	}
}

// ==============================================

void AWallRunnerCharacter::PlayerReady()
{
	auto GState = Cast<ATestGameState>(GetWorld()->GetGameState());
	int RCount = GState->RedTeam->GetPlayersCount();
	int BCount = GState->BlueTeam->GetPlayersCount();
	if (!IsReady && (BCount == 0 || RCount == 0) && RCount + BCount > 1)
	{
		if (GameInfoWidget)
		{
			GameInfoWidget->SetErrorMessage("Unable to start the game, while there is an empty team");
		}
		return;
	}

	IsReady = !IsReady;
	ServerPlayerReady(IsReady);
}

void AWallRunnerCharacter::ServerPlayerReady_Implementation(bool bReady)
{
	auto GState = Cast<ATestGameState>(GetWorld()->GetGameState());
	GState->OnTogglePlayerReady(GetPlayerState(), bReady);
}

void AWallRunnerCharacter::ResetRotation_Implementation(const FRotator& Rotator)
{
	GetController()->SetControlRotation(Rotator);
}
