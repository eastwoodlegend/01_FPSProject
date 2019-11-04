// Will Pechell

#include "FPSProject.h"
#include "FPSPlayerController.h"
#include "FPSCharacter.h"



AFPSCharacter* AFPSPlayerController::GetControlledCharacter() const
{
	return Cast<AFPSCharacter>(GetPawn());
}