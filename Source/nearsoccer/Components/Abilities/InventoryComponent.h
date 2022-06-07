// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "BuffInfo.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class AWallRunnerCharacter;
class UBaseBuff;
class ANearGrenade;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEARSOCCER_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Type of PowerUp in inventory
	UPROPERTY(Replicated)
	TSubclassOf<UBaseBuff> PowerUpType;
	
	// Type of Grenade in inventory
	UPROPERTY(Replicated)
	TSubclassOf<ANearGrenade> GrenadeType;
	
	// Throwing grenade sound
	UPROPERTY(EditAnywhere)
	USoundBase* ThrowSound;
	
	// Using PowerUp sound
	UPROPERTY(EditAnywhere)
	USoundBase* PowerUpSound;

public:

	// Sets player's inventory PowerUp type
	void SetPowerUpType(TSubclassOf<UBaseBuff> Value);

	// Activating buff
	void ActivatePowerUp();
	
	FBuffInfo GetPowerUpInfo();

	// Sets player's inventory PowerUp type
	void SetGrenadeType(TSubclassOf<ANearGrenade> Value);

	// Activating buff
	void ThrowGrenade(FVector Location, FRotator Rotation);
	
	FBuffInfo GetGrenadeInfo();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void ServerSetPowerUpType(TSubclassOf<UBaseBuff> Value);
	
	UFUNCTION(Server, Reliable)
	void ServerActivatePowerUp();
	
	UFUNCTION(Server, Reliable)
	void ServerSetGrenadeType(TSubclassOf<ANearGrenade> Value);
	
	UFUNCTION(Server, Reliable)
	void ServerThrowGrenade(FVector Location, FRotator Rotation);
	
	UFUNCTION(NetMulticast, Reliable)
	void MultiSpawnThrowSound(ACharacter* Character);
};
