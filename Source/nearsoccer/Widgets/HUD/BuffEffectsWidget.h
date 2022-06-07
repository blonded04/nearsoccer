// Nearsoccer 2022

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuffEffectsWidget.generated.h"

/**
 * 
 */
UCLASS()
class NEARSOCCER_API UBuffEffectsWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UHorizontalBox* BuffBox;
	
};
