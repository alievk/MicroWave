// K. A. Aliev

#include "MicroWave.h"

AMWPlayerController::AMWPlayerController(const FObjectInitializer& OI)
	: Super(OI)
{
	MWCharacter = nullptr;
}

void AMWPlayerController::SetPawn(APawn* InPawn)
{
	AController::SetPawn(InPawn);

	MWCharacter = Cast<AMWCharacter>(InPawn);
}

// ///////////////////////////////////////////////////
// Backup

//void AMWPlayerController::PlayerTick(float DeltaTime)
//{
//	Super::PlayerTick(DeltaTime);
//
//}
//
//void AMWPlayerController::SetupInputComponent()
//{
//	Super::SetupInputComponent();  // initialize InputComponent
//
//	// Jumping is handled within pawn blueprint
////	InputComponent->BindAction("Jump", IE_Pressed, this, &AMWPlayerController::OnJump);
//
//	InputComponent->BindAction("Fire", IE_Pressed, this, &AMWPlayerController::OnFire);
//	InputComponent->BindAction("AltFire", IE_Pressed, this, &AMWPlayerController::OnAltFire);
//	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AMWPlayerController::OnTouchStarted);
//
//	InputComponent->BindAxis("Turn", this, &APlayerController::AddYawInput);
//	InputComponent->BindAxis("LookUp", this, &APlayerController::AddPitchInput);
//
//	InputComponent->BindAxis("VerticalAxis", this, &AMWPlayerController::OnVerticalAxis);
//	InputComponent->BindAxis("HorisontalAxis", this, &AMWPlayerController::OnHorisontalAxis);
//}

//void AMWPlayerController::OnTouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	// only fire for first finger down
//	if (FingerIndex == 0)
//	{
//		OnFire();
//	}
//}
//
//void AMWPlayerController::OnFire()
//{
//	if (MWCharacter)
//	{
//		MWCharacter->UseInventory();
//	}
//}
//
//void AMWPlayerController::OnAltFire()
//{
//	if (MWCharacter)
//	{
//		MWCharacter->UseInventory();
//	}
//}
//void AMWPlayerController::OnJump()
//{
//	if (MWCharacter)
//	{
//		MWCharacter->Jump();
//	}
//}
//
//void AMWPlayerController::OnVerticalAxis(float Scale)
//{
//	if (MWCharacter)
//	{
//		MWCharacter->MoveForward(Scale);
//	}
//}
//
//void AMWPlayerController::OnHorisontalAxis(float Scale)
//{
//	if (MWCharacter)
//	{
//		MWCharacter->MoveRight(Scale);
//	}
//}

//InputComponent->BindAction("RotateCameraCW", IE_Pressed, this, &AMWPlayerController::OnRotateCameraCWPressed);
//InputComponent->BindAction("RotateCameraCCW", IE_Pressed, this, &AMWPlayerController::OnRotateCameraCCWPressed);

//void AMWPlayerController::UpdatePawnAim()
//{
//	if (!MWCharacter)
//	{
//		return;
//	}
//	
//	FHitResult Hit;
//
//	// Cast ray from the camera view to the point under the cursor
//	GetHitResultUnderCursor(ECC_Camera, false, Hit);
//	
//	if (Hit.bBlockingHit) // hit something?
//	{
//		MWCharacter->SetAim(Hit.ImpactPoint);
//	}
//}

//void AMWPlayerController::OnRotateCameraCWPressed()
//{
//	if (MWCharacter)
//	{
//		MWCharacter->Turn(90.f);
//		/*MWCharacter->RotateCameraCW(true);
//		CameraAngle = ECameraAngle::Right == CameraAngle ? 
//			ECameraAngle::Normal : CameraAngle + 1;*/
//	}
//}
//
//void AMWPlayerController::OnRotateCameraCCWPressed()
//{
//	if (MWCharacter)
//	{
//		MWCharacter->Turn(-90.f);
//		//MWCharacter->RotateCameraCW(false);
//		//CameraAngle = ECameraAngle::Normal == CameraAngle ?
//		//	ECameraAngle::Right : CameraAngle - 1;
//	}
//}

//void AMWPlayerController::OnTurnAtRate(float Scale)
//{
//	// calculate delta for this frame from the rate information
//	AddYawInput(Scale * BaseTurnRate * GetWorld()->GetDeltaSeconds());
//}
//
//void AMWPlayerController::OnLookUpAtRate(float Scale)
//{
//	// calculate delta for this frame from the rate information
//	AddPitchInput(Scale * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
//}

//switch (CameraAngle)
//{
//case ECameraAngle::Left:
//	MWCharacter->MoveRight(Scale);
//	break;
//case ECameraAngle::Front:
//	MWCharacter->MoveForward(-Scale);
//	break;
//case ECameraAngle::Right:
//	MWCharacter->MoveRight(-Scale);
//	break;
//default:
//	MWCharacter->MoveForward(Scale);
//	break;
//}

//switch (CameraAngle)
//{
//case ECameraAngle::Left:
//	MWCharacter->MoveForward(-Scale);
//	break;
//case ECameraAngle::Front:
//	MWCharacter->MoveRight(-Scale);
//	break;
//case ECameraAngle::Right:
//	MWCharacter->MoveForward(Scale);
//	break;
//default:
//	MWCharacter->MoveRight(Scale);
//	break;
//}

// We have 2 versions of the rotation bindings to handle different kinds of devices differently
// "turn" handles devices that provide an absolute delta, such as a mouse.
// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
/*InputComponent->BindAxis("Turn", this, &APlayerController::AddYawInput);
InputComponent->BindAxis("TurnRate", this, &AMWPlayerController::OnTurnAtRate);

InputComponent->BindAxis("LookUp", this, &APlayerController::AddPitchInput);
InputComponent->BindAxis("LookUpRate", this, &AMWPlayerController::OnLookUpAtRate);*/

//FString str;
//if (Hit.GetActor())
//{
//	str += Hit.GetActor()->GetHumanReadableName();
//	str += ":";
//}
//if (Hit.GetComponent())
//{
//	str += Hit.GetComponent()->GetName();
//}
//
//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, str);

//if (Cast<UMWPickableSphere>(Hit.GetComponent())) // hit pickable sphere?
//if (Cast<UStaticMeshComponent>(Hit.GetComponent()))
//{
//	Hit.Actor->bHidden = !Hit.Actor->bHidden;

//	FVector newScale(3, 3, 3);
//	Hit.Actor->SetActorScale3D(newScale);
//}

// Backup
// ///////////////////////////////////////////////////
