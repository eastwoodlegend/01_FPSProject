// Will Pechell

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FPSHUD.generated.h"

/**
 * 
 */
UCLASS()
class FPSPROJECT_API AFPSHUD : public AHUD
{
	GENERATED_BODY()

public:

	virtual void DrawHUD() override;
	
protected:

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* CrosshairTexture;
};
