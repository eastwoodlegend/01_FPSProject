// Will Pechell

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSPlayerController.generated.h"


class AFPSCharacter;
/**
 * 
 */
UCLASS()
class FPSPROJECT_API AFPSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	
	AFPSCharacter* GetControlledCharacter() const;
	
};
