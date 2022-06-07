// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Trampoline.generated.h"

class UBoxComponent;

UCLASS()
class NEARSOCCER_API ATrampoline : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrampoline();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComp;
	
};
