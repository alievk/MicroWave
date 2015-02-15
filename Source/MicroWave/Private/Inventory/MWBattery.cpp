// K. A. Aliev

#include "MicroWave.h"

AMWBattery::AMWBattery(const FObjectInitializer& OI)
	: Super(OI)
{
	// BatteryMesh
	BatteryMesh = OI.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("BatteryMesh"));
	RootComponent = BatteryMesh;

	// LEDs
	for (uint32 i = 0; i < kMaxLEDs; ++i)
	{
		FString ledName("LED");
		ledName.AppendInt(i + 1);
		LEDs[i] = OI.CreateDefaultSubobject<UStaticMeshComponent>(this, *ledName);
	}

	bWantsPowerUp = false;

	// Enable ticking actor
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
}

void AMWBattery::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	for (uint32 i = 0; i < kMaxLEDs; ++i)
	{
		// Attach to its point on the battery mesh
		FString name("led");
		name.AppendInt(i + 1);
		FName boneName(*name);
		LEDs[i]->AttachTo(BatteryMesh, boneName, EAttachLocation::SnapToTarget);

		// Assign common LED static mesh and create dynamic material. 
		LEDs[i]->StaticMesh = LEDMesh;
		LED_MIDs[i] = LEDs[i]->CreateAndSetMaterialInstanceDynamic(0);
	}

	BatteryMID = BatteryMesh->CreateAndSetMaterialInstanceDynamic(0);

	CurrentCharge = Capacity;
	CurrentPower = NormalPower;

	UpdateLEDs();
}

void AMWBattery::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bWantsPowerUp)
	{
		CurrentPower = FMath::Min(MaxPower, CurrentPower + PowerUpRate * DeltaTime);
	}
	else
	{
		float powerDownRate = PowerUpRate * 2;
		CurrentPower = FMath::Max(NormalPower, CurrentPower - powerDownRate * DeltaTime);
	}

	UpdateLEDs();
}

void AMWBattery::OnPowerUpStart()
{
	bWantsPowerUp = true;
}

void AMWBattery::OnPowerUpEnd()
{
	bWantsPowerUp = false;
}

void AMWBattery::OnEquip()
{
	if (!MyPawn)
	{
		return;
	}

	// Attach battery to the owner
	FName AttachPoint = MyPawn->GetBatterySocketName();
	USkeletalMeshComponent* PawnMesh = MyPawn->GetMesh();
	BatteryMesh->AttachTo(PawnMesh, AttachPoint, EAttachLocation::SnapToTarget);
}

float AMWBattery::ConsumeCharge(float DeltaTime)
{
	const float oldCharge = CurrentCharge;
	CurrentCharge = FMath::Max(-0.01f, CurrentCharge - CurrentPower * DeltaTime);

	return oldCharge - CurrentCharge;
}

void AMWBattery::RestoreCharge(float Charge)
{
	CurrentCharge = FMath::Min(Capacity, CurrentCharge + Charge);
}

void AMWBattery::SetOwningPawn(AMWCharacter* Pawn)
{
	MyPawn = Pawn;
}

float AMWBattery::GetCurrentCharge() const
{ 
	return CurrentCharge;
}

float AMWBattery::GetCurrentPower() const
{ 
	return HasCharge() ? CurrentPower : 0.f;
}

bool AMWBattery::HasCharge() const
{
	return CurrentCharge > 0.f;
}

void AMWBattery::UpdateLEDs()
{
	const float chargeLEDs = kMaxLEDs * CurrentCharge / Capacity;
	const float powerLEDs = kMaxLEDs * CurrentPower / MaxPower;

	for (uint32 i = 0; i < kMaxLEDs; ++i)
	{
		if (!LED_MIDs[i])
		{
			continue;
		}

		if (i <= chargeLEDs)
		{
			LED_MIDs[i]->SetScalarParameterValue(TEXT("ChargeOn"), 1.f);
		}
		else
		{
			LED_MIDs[i]->SetScalarParameterValue(TEXT("ChargeOn"), 0.f);
		}

		if (i <= powerLEDs)
		{
			LED_MIDs[i]->SetScalarParameterValue(TEXT("PowerUpOn"), 1.f);
		}
		else
		{
			LED_MIDs[i]->SetScalarParameterValue(TEXT("PowerUpOn"), 0.f);
		}
	}

	// Turn off/on MW label on the battery when the power is maximum.
	if (FMath::IsNearlyEqual(CurrentPower, MaxPower, 0.01f))
	{
		if (BatteryMID)
		{
			BatteryMID->SetScalarParameterValue(TEXT("MWOn"), 1.f);
		}
	}
	else
	{
		if (BatteryMID)
		{
			BatteryMID->SetScalarParameterValue(TEXT("MWOn"), 0.f);
		}
	}
}
