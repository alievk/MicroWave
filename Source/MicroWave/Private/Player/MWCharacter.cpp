// K. A. Aliev

#include "MicroWave.h"
//#include "CableComponent.h"
//#include "ModuleManager.h"

//////////////////////////////////////////////////////////////////////////
// AMWCharacter

AMWCharacter::AMWCharacter(const FObjectInitializer& OI)
	: Super(OI)
{
	// Aiming animation requires the controller doesnt affect pawn's yaw
	bUseControllerRotationYaw = false;

	// Movement
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = true; // Orient the character along the moving direction
	//Movement->RotationRate = FRotator(0.f, 640.f, 0.f);
	//Movement->bConstrainToPlane = true;
	//Movement->bSnapToPlaneAtStart = true;

	// Capsule component
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Camera boom
	CameraBoom = OI.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want boom to rotate when character does
	CameraBoom->bDoCollisionTest = true; // Adjust boom length when the pawn occluded by geometry
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->TargetArmLength = 160.f;
	CameraBoom->SocketOffset = FVector(0.f, 28.f, 82.f);

	// Top-down camera
	CameraComponent = OI.CreateDefaultSubobject<UCameraComponent>(this, TEXT("Camera"));
	CameraComponent->AttachTo(CameraBoom, USpringArmComponent::SocketName);

	bTurnBodyToAim = true;
	Health = 100;
}

void AMWCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Add & equip the default MWGun
	if (*DefaultMWGun)
	{
		AddGun(DefaultMWGun);
	}
}

void AMWCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bTurnBodyToAim)
	{
		TurnToAim();
	}
}

void AMWCharacter::AddGun(TSubclassOf<class AMWGun> GunClass)
{
	// Spawn and equip new gun

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.bNoCollisionFail = true;
	AMWGun* newGun = GetWorld()->SpawnActor<AMWGun>(GunClass, SpawnInfo);

	Inventory.AddUnique(newGun);
	EquipGun(newGun);
}

void AMWCharacter::EquipGun(class AMWGun* Gun)
{
	check(Gun)
	Gun->SetOwningPawn(this);
	Gun->OnEquip();

	AMWBattery* battery = Gun->GetBattery();
	if (battery)
	{
		battery->SetOwningPawn(this);
		battery->OnEquip();
	}

	CurrentGun = Gun;
}

AMWGun* AMWCharacter::GetGun() const
{
	return CurrentGun;
}

FName AMWCharacter::GetGunSocketName() const
{
	return GunSocketName;
}

FName AMWCharacter::GetBatterySocketName() const
{
	return BatterySocketName;
}

//////////////////////////////////////////////////////////////////////////
// Control

void AMWCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	check(InputComponent);

	// Jumping is handled within pawn blueprint
	//InputComponent->BindAction("Jump", IE_Pressed, this, &AMWCharacter::OnJump);

	InputComponent->BindAction("StartFire", IE_Pressed, this, &AMWCharacter::OnStartFire);
	InputComponent->BindAction("StartFire", IE_Released, this, &AMWCharacter::OnStopFire);
	InputComponent->BindAction("StartAltFire", IE_Pressed, this, &AMWCharacter::OnStartAltFire);
	InputComponent->BindAction("StartAltFire", IE_Released, this, &AMWCharacter::OnStopFire);


	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	InputComponent->BindAxis("MoveForward", this, &AMWCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AMWCharacter::MoveRight);
}

void AMWCharacter::OnStartFire()
{
	if (CurrentGun)
	{
		CurrentGun->StartFire();
	}
}

void AMWCharacter::OnStartAltFire()
{
	if (CurrentGun)
	{
		CurrentGun->StartAltFire();
	}
}

void AMWCharacter::OnStopFire()
{
	if (CurrentGun)
	{
		CurrentGun->StopFire();
	}
}

void AMWCharacter::MoveForward(float Scale)
{
	if (Scale != 0.0f)
	{
		FVector forwardDir = GetControlRotation().Vector();
		AddMovementInput(forwardDir, Scale);
	}
}

void AMWCharacter::MoveRight(float Scale)
{
	if (Scale != 0.0f)
	{
		FVector rightDir = FVector(0.f, 0.f, 1.f) ^ GetControlRotation().Vector();
		AddMovementInput(rightDir, Scale);
	}
}

bool AMWCharacter::Hurt(int32 Pain)
{
	Health = FMath::Max(0, Health - FMath::Max(0, Pain));

	return Health > 0;
}

bool AMWCharacter::IsAlive() const
{ 
	return Health > 0;
}

bool AMWCharacter::IsNoisy() const
{
	// noisy if moving
	return GetVelocity().Size() > 10.f;
}

void AMWCharacter::TurnToAim()
{
	const float aimLimit = 90; // aiming angle limit (defined by the animation)
	const FRotator ctrRot = GetControlRotation();
	const FRotator actRot = GetActorRotation();
	FRotator delta = (ctrRot - actRot).GetNormalized();

	if (delta.Yaw > aimLimit)
	{
		FRotator cwDelta(0.f, delta.Yaw - 90.f, 0.f);
		AddActorWorldRotation(cwDelta);
	}
	else if (delta.Yaw < -aimLimit)
	{
		FRotator ccwDelta(0.f, delta.Yaw + 90.f, 0.f);
		AddActorWorldRotation(ccwDelta);
	}
}

// ///////////////////////////////////////////////////
// Backup

//void AMWCharacter::UseInventory()
//{
// try and fire a projectile
//if (ProjectileClass != NULL)
//{
//	const FRotator SpawnRotation = GetControlRotation();
//	// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
//	const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(GunOffset);

//	UWorld* const World = GetWorld();
//	if (World != NULL)
//	{
//		// spawn the projectile at the muzzle
//		World->SpawnActor<AMWProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
//	}
//}

//// try and play the sound if specified
//if (FireSound != NULL)
//{
//	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
//}

// try and play a firing animation if specified
//if(FireAnimation != NULL)
//{
//	// Get the animation object for the arms mesh
//	UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
//	if(AnimInstance != NULL)
//	{
//		AnimInstance->Montage_Play(FireAnimation, 1.f);
//	}
//}
//}

// Cable component
//IModuleInterface unused = FModuleManager::LoadModuleChecked<IModuleInterface>(TEXT("CableComponent"));
//CableComponent = OI.CreateOptionalDefaultSubobject<UCableComponent>(this, TEXT("Cable"));

// CableRoot. Set the offset in the archetype.
//CableRoot = OI.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("CableRoot"));
//CableRoot->AttachParent = BatteryMesh;

//// CableEnd1. Snapped to body mesh socket.
//CableEnd1 = OI.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("CableEnd1"));

//// CableEnd2. Snapped to gun mesh socket.
//CableEnd2 = OI.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("CableEnd2"));

// Attach the 1st cable ending to the body mesh socket
//CableEnd1->AttachTo(GetMesh(), CableTouchSocketName, EAttachLocation::SnapToTarget);

//// Attach the 2nd cable ending to the gun (gun's root component must be a mesh with the socket!)
//CableEnd2->AttachTo(GetGun()->GetRootComponent(), GetGun()->CableSocketName, EAttachLocation::SnapToTarget);

// Attach cable's rood and ending
//CableComponent->AttachParent = CableRoot;
//CableComponent->AttachEndTo.OverrideComponent = CableEnd;

// Backup
// ///////////////////////////////////////////////////
