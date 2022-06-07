// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "ForbiddenStaticMeshActor.generated.h"

UCLASS()
class NEARSOCCER_API AForbiddenStaticMeshActor : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AForbiddenStaticMeshActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
