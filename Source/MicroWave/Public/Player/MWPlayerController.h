// K. A. Aliev

#pragma once

#include "GameFramework/PlayerController.h"
#include "MWPlayerController.generated.h"

/**
 * MicroWave player controller.
 */
UCLASS(Config=Game)
class MICROWAVE_API AMWPlayerController : public APlayerController
{
	GENERATED_UCLASS_BODY()

protected:

	// Begin AController Interface
	virtual void SetPawn(APawn* InPawn) override;
	// End AController Interface

protected:
	/** MWCharacter being controlled by MWPlayerController */
	UPROPERTY(Transient)
	class AMWCharacter* MWCharacter;

};


// ///////////////////////////////////////////////////////////
// Backup

//// Begin APlayerController Interface
//virtual void PlayerTick(float DeltaTime) override;
//virtual void SetupInputComponent() override;
//// End APlayerController Interface
//
//// //////////////////////////////////////////////
//// Input
//
///**  */
//void OnTouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location);
//
///** Fires a projectile. */
//void OnFire();
//
///** Makes pawn to jump */
//void OnJump();
//
///** Handles pressing W/S or Up/Down */
//void OnVerticalAxis(float Scale);
//
///** Handles pressing A/E or Left/Right */
//void OnHorisontalAxis(float Scale);

/** Top-down camera angles */
//namespace ECameraAngle
//{
//	const uint8 Normal = 1;
//	const uint8 Left = 2;
//	const uint8 Front = 3;
//	const uint8 Right = 4;
//}

/** Yaws pawn */
//void OnTurn();

/**
* Called via input to turn at a given rate.
* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
*/
//void OnTurnAtRate(float Scale);

/** Pitches pawn */
//void OnLookUp();

/**
* Called via input to turn look up/down at a given rate.
* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
*/
//void OnLookUpAtRate(float Scale);

/** Rotates pawn's boom camera CW */
//void OnRotateCameraCWPressed();

///** Rotates pawn's boom camera CCW */
//void OnRotateCameraCCWPressed();

///** Update the direction the owned pawn aims in */
//void UpdatePawnAim();

// ///////////////////////////////////////////////////////////////
// Control

/** Camera angle
* @see ECameraAngle
*/
//uint8 CameraAngle;

///** Normalized pawn aiming vertor */
//UPROPERTY(BlueprintReadOnly, Category=Control)
//FVector Aiming;

///** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
//float BaseTurnRate;

///** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
//float BaseLookUpRate;