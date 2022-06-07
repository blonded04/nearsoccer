// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "BuffComponent.h"
#include "BuffInfo.h"
#include "EscapeWidget.h"
#include "InventoryComponent.h"
#include "GameInfoWidget.h"
#include "NiagaraCommon.h"
#include "SpawnTimerWidget.h"
#include "TeamSelectWidget.h"
#include "Widgets/HUD/BuffEffectsWidget.h"
#include "Widgets/HUD/FlashScreen.h"
#include "Widgets/HUD/CooldownWidget.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Widgets/HUD/Scoreboard.h"
#include "Components/Abilities/WallRunComponent.h"
#include "Widgets/HUD/SpeedWidget.h"
#include "WallRunnerCharacter.generated.h"

UCLASS()
class NEARSOCCER_API AWallRunnerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Public variables and fields
	// Sets default values for this character's properties
	AWallRunnerCharacter();

	// Draw all the HUD
	UFUNCTION(BlueprintCallable)
	void DrawHUDWidgets();

	UFUNCTION(Client, Reliable)
	void ChangeStencil(ACharacter* OtherCharacter, int Stencil);

	// Destroy all the HUD
	UFUNCTION(BlueprintCallable)
	void DestroyHUDWidgets(bool bDestroyGameInfo = true);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ChangeScoreUI(ETeam ScoredTeam, int NewScore);

	UFUNCTION()
	void RebuildScoreboard();

	UFUNCTION()
	void UpdateKillFeed(const FString& Killer, const FString& Killed, EDeathType DeathType, bool bWasKilled = false);

	UFUNCTION()
	void SetWinner(EWinnerType Winner);

	// Process prop hit
	UFUNCTION()
	virtual void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	// Allow our character to double-jump
	UPROPERTY(EditAnywhere, Category=Mobility)
	bool bDoubleJumping{true};

	// Default air-control
	UPROPERTY(EditAnywhere, Category=Mobility)
	float AirControl{1.0f};

	// Base look up/down rate, in deg/sec. Other scaling may affect final rate.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate{45.0f};

	// Base turn rate, in deg/sec. Other scaling may affect final turn rate.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate{45.0f};

	// First person camera
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UCameraComponent* FPSCameraComponent;

	// First person mesh
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UMeshComponent* FPSMesh;

	// Third person gun mesh
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UMeshComponent* TPSGunMesh;

	// Copy of GetMesh()
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UMeshComponent* FXMesh;

	// Location on TPS gun mesh where grenades should spawn.
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* GrenadeSpawnLocation;

	// Weapon that player holds at the moment
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class ACombatWeapon> WeaponClass;
	UPROPERTY(Replicated)
	ACombatWeapon* Weapon{nullptr};

	// WallRide logics
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	UWallRunComponent* WallRunComponent;

	// Dash logics
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	UDashComponent* DashComponent;

	// Inventory logics
	UPROPERTY(EditAnywhere)
	UInventoryComponent* InventoryComp;

	// Buff logics
	UPROPERTY(EditAnywhere)
	UBuffComponent* BuffComp;
	
	// Niagara component responsible for passing info whether primary fire is available
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraComponent* IndicatorPrimaryFire;

	// Niagara component responsible for passing info whether secondary fire is available
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraComponent* IndicatorSecondaryFire;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD | Scoreboard")
	TSubclassOf<UScoreboard> ScoreboardClass;
	UPROPERTY()
	UScoreboard* ScoreboardWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD | Escape")
	TSubclassOf<UTeamSelectWidget> TeamSelectClass;
	UPROPERTY()
	UTeamSelectWidget* TeamSelectWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD | Escape")
	TSubclassOf<UEscapeWidget> EscapeClass;
	UPROPERTY()
	UEscapeWidget* EscapeWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD | SpawnTimer")
	TSubclassOf<USpawnTimerWidget> SpawnTimerClass;
	UPROPERTY()
	USpawnTimerWidget* SpawnTimerWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD | GameInfo")
	TSubclassOf<UGameInfoWidget> GameInfoClass;
	UPROPERTY()
	UGameInfoWidget* GameInfoWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD | In-game")
	TSubclassOf<USpeedWidget> SpeedClass;
	UPROPERTY()
	USpeedWidget* SpeedWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD | In-game")
	TSubclassOf<UCooldownWidget> CooldownClass;
	UPROPERTY()
	UCooldownWidget* CooldownWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD | In-game")
	TSubclassOf<UBuffEffectsWidget> BuffWidgetClass;
	UPROPERTY()
	UBuffEffectsWidget* BuffWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD | In-game")
	TSubclassOf<UFlashScreen> FlashScreenClass;
	UPROPERTY()
	UFlashScreen* FlashScreen;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD | In-game")
	TSubclassOf<UUserWidget> HUDClass;
	UPROPERTY()
	UUserWidget* HUDWidget;

	UFUNCTION(BlueprintCallable)
	void ToggleEscape();

	UFUNCTION(BlueprintCallable)
	void ToggleTeamSelect();

protected:
	// Protected variables and fields
	UPROPERTY(ReplicatedUsing=OnRep_PlayerTeamId)
	int PlayerTeamId;

	UFUNCTION()
	void OnRep_PlayerTeamId();

	// Timer handles
	FTimerHandle RespawnHandle;
	FTimerHandle BumpHandle;

	// Player base walk speed
	UPROPERTY(EditDefaultsOnly, Category="Speed")
	float DefaultMaxMoveSpeed{600.0f};

	UPROPERTY(EditDefaultsOnly, Category="FX")
	UNiagaraSystem* CircleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FX")
	UNiagaraSystem* MeshEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SFX")
	USoundBase* EarRingSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SFX")
	USoundBase* DeathSound;

	// Is player ready to start the game
	UPROPERTY(Replicated)
	bool IsReady = false;

	// Current player walk speed
	UPROPERTY(Replicated)
	float CurrentMaxMoveSpeed{DefaultMaxMoveSpeed};

	// true, if player should die after shield ends
	UPROPERTY(Replicated)
	bool bIsInLava{false};

	// true, if player already dead
	UPROPERTY(Replicated)
	bool bIsDead{false};

	UPROPERTY(Replicated)
	int FXSpawned{0};

	// Responsible for the current state of the flash screen
	UPROPERTY(Replicated, EditDefaultsOnly)
	float FlashOpacity{0.0f};

public:
	// ==================SETTING_UP==================

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// ===================MOVEMENT===================

	// Add bouncing off the floor after high speed fall
	virtual void Landed(const FHitResult& Hit) override;

	// Updates max movement speed
	void UpdateMaxSpeed();

	virtual void Jump() override;

	virtual void AddControllerPitchInput(float Val) override;

	virtual void AddControllerYawInput(float Val) override;

	void BeginRiding();

	void StopRiding();

	void DashInput();

	UFUNCTION(BlueprintCallable)
	bool IsRiding() const { return WallRunComponent->IsRiding(); }

	// ====================WIDGETS===================

	UFUNCTION(BlueprintCallable)
	const TArray<FBuffInfo>& GetBuffsInfoArray();

	UFUNCTION(BlueprintCallable)
	FBuffInfo GetPowerUpInfo() const;

	UFUNCTION(BlueprintCallable)
	FBuffInfo GetGrenadeInfo() const;

	// ==============BUFFS_AND_INVENTORY=============

	// Updates FlashOpacity variable
	UFUNCTION(BlueprintCallable)
	void SetFlashOpacity(float Value);

	UFUNCTION(BlueprintNativeEvent)
	void SpawnUISound(USoundBase* Sound, float SoundStartPoint);

	UFUNCTION(BlueprintCallable)
	float GetFlashOpacity() { return FlashOpacity; }

	// Updates bIsInLava variable
	UFUNCTION(BlueprintCallable)
	void UpdateIsInLava(bool Value);

	bool GetIsInLava() const { return bIsInLava; }

	// Updates FlashOpacity variable
	UFUNCTION(BlueprintCallable)
	void SetImpulseVector(FVector Vec);

	void SpawnFX(FBuffInfo Buff);

	// ====================DAMAGE====================

	// Takes damage from blueprint scripts
	UFUNCTION(BlueprintCallable)
	void GetHurt(bool bIsServerCalled, ATestPlayerState* Killer = nullptr, EDeathType DeathType = ELava,
	             ATestPlayerState* KilledPlayerState = nullptr);

	// ================TEAM_AND_COLOR================

	// Sets player color
	void SetTeamId(int TeamId);


	FLinearColor GetPlayerColor() const
	{
		switch (PlayerTeamId)
		{
		case 1:
			return FLinearColor(0.3f, 0.3f, 1.0f, 1);
		case 2:
			return FLinearColor(1.0f, 0.3f, 0.3f, 1);
		}
		return FLinearColor(0.0f, 0.0f, 0.0f, 1);
	}

	UFUNCTION(BlueprintCallable)
	ETeam GetPlayerTeam() const
	{
		switch (PlayerTeamId)
		{
		case 1:
			return ETeam::EBlue;
		case 2:
			return ETeam::ERed;
		}
		return ETeam::ENone;
	}

	// ==============================================

	UFUNCTION(Client, Reliable)
	void SetSpawnTimerValue(int TimeLeft);

	UFUNCTION(Client, Reliable)
	void SetGameTimerValue(int TimeLeft);
protected:
	// Protected functions
	// ==================SETTING_UP==================

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ===================MOVEMENT===================

	// Bounce after fall implementation
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Bounce(AWallRunnerCharacter* Player, const FVector& Vec);

	// Handles input for moving forward and backward.
	UFUNCTION()
	void MoveForward(float Val);

	// Handles input for moving right and left.
	UFUNCTION()
	void MoveRight(float Val);

	// Updates max movement speed on server
	UFUNCTION(Server, Reliable)
	void ServerUpdateMaxSpeed();

	// Starts crouching
	UFUNCTION()
	void BeginCrouching();

	// Stops crouching
	UFUNCTION()
	void StopCrouching();

	// Called via input to turn at a given rate.
	// @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	void TurnAtRate(float Rate);

	// Called via input to turn look up/down at a given rate.
	// @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	void LookUpAtRate(float Rate);

	// ====================WIDGETS===================

	void OpenScoreboard();

	void DestroyScoreboard();

	// Test info for HUD: inventory + active buffs
	UFUNCTION(BlueprintPure)
	FString GetHUDInfo() const;

	// ==============BUFFS_AND_INVENTORY=============

	UFUNCTION(Server, Reliable)
	void ServerSetFlashOpacity(float Value);

	UFUNCTION(NetMulticast, Reliable)
	void MultiSpawnFlashSound(float SoundStartPoint);

	UFUNCTION(Server, Reliable)
	void ServerUpdateIsInLava(bool Value);

	UFUNCTION(Server, Reliable)
	void ServerSetImpulseVector(FVector Vec);

	UFUNCTION(Server, Reliable)
	void ServerSpawnFX(FBuffInfo Buff);

	UFUNCTION(NetMulticast, Reliable)
	void MultiSpawnMeshEffect(FBuffInfo Buff);

	UFUNCTION(BlueprintNativeEvent)
	void SetUpMesh(FBuffInfo Buff);

	UFUNCTION(NetMulticast, Reliable)
	void MultiSpawnCircleEffect(FBuffInfo Buff);

	// Activates buff from PowerUp
	void ActivatePowerUp();

	UFUNCTION(Server, Reliable)
	void ServerActivatePowerUp();

	// Throwing grenade
	void ThrowGrenade();

	UFUNCTION(Server, Reliable)
	void ServerThrowGrenade();

	// ====================DAMAGE====================

	// Call GetHurt via button
	void GetHurtBind();

	// Player gets hurt (fatal damage)
	UFUNCTION(Server, Reliable)
	void ServerGetHurt(ATestPlayerState* Killer, EDeathType DeathType,
	                   ATestPlayerState* KilledPlayerState);

	// Dying function: calls multicast dying and timer for destroying
	void Die();

	// Multicast dying function
	UFUNCTION(NetMulticast, Reliable)
	void MultiDie();

	// Destroys player
	void CallDestroy();

	// ================TEAM_AND_COLOR================

	UFUNCTION(Server, Reliable)
	void ServerSetTeamId(int TeamId);

	UFUNCTION(Server, Reliable)
	void ChangeNameServer(const FString& Name);

	// ===========Weapon-related functions===========

	UFUNCTION(BlueprintCallable)
	void WeaponPrimaryFireBegin();

	UFUNCTION(BlueprintCallable)
	void WeaponPrimaryFireEnd();

	UFUNCTION(BlueprintCallable)
	void WeaponSecondaryFireBegin();

	UFUNCTION(BlueprintCallable)
	void WeaponSecondaryFireEnd();

	UFUNCTION(BlueprintCallable)
	void WeaponReload();

	// ==============================================

	// ================Hub logic================

	void PlayerReady();

	UFUNCTION(Server, Reliable)
	void ServerPlayerReady(bool bReady);

	// Time after LastPushed resets
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gamemode")
	float TimePusherRemainsAfterPush{5.0f};

	void WrapNameWithTeamTag(FString& Name, ETeam TeamType);

public:
	void ChangeName(const FString& Name);

	UFUNCTION(Server, Reliable)
	void ChangeTeamServer();

	UFUNCTION(Client, Reliable)
	void ResetRotation(const FRotator& Rotator);

private:
	// Possible killer who pushed character to lava via weapon
	TOptional<ATestPlayerState*> LastPushed;

	void ResetLastPushed()
	{
		check(HasAuthority());
		LastPushed.Reset();
	}

	// Handle responsible to time after LastPushed resets
	FTimerHandle HandlePusherRemainsAfterPush;
	
	UFUNCTION(BlueprintCallable)
	void LeaveServer();
};
