

#include "MicroWave.h"

AMWGameMode::AMWGameMode(const FObjectInitializer& OI)
	: Super(OI)
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Character/Burglar"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AMWHUD::StaticClass();

	PlayerControllerClass = AMWPlayerController::StaticClass();
}
