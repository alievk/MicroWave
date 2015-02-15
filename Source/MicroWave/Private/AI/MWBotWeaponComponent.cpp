// K. A. Aliev

#include "MicroWave.h"

UMWBotWeaponComponent::UMWBotWeaponComponent(const FObjectInitializer& OI)
	: Super(OI)
{
	FirePeriod = 0.1f;
	BurstPeriod = 0.8f;
	BurstCount = 3;
	Spread = 3.f;
	Strength = 20;
}

void UMWBotWeaponComponent::Fire()
{
	if (!CanFire())
	{
		return;
	}

	// cast a line
	FHitResult hit;
	FCollisionQueryParams traceParams;
	traceParams.AddIgnoredActor(GetOwner());

	const int32 randSeed = FMath::Rand();
	FRandomStream randStream(randSeed);
	
	const FVector traceStart = GetComponentLocation();
	const FVector aimDir = GetComponentRotation().Vector();
	const FVector shotDir = randStream.VRandCone(aimDir, FMath::DegreesToRadians(Spread));
	const FVector traceEnd = traceStart + shotDir * 1e5;

	GetWorld()->LineTraceSingle(hit, traceStart, traceEnd, ECollisionChannel::ECC_Visibility, traceParams);

	ProcessHit(hit);

	// temporary draw
	
	if (hit.bBlockingHit)
	{
		DrawDebugLine(GetWorld(), traceStart, hit.ImpactPoint, FColor::Red, false, -1.f, 0, 10);
	}
	else
	{
		DrawDebugLine(GetWorld(), traceStart, traceEnd, FColor::Red, false, -1.f, 0, 10);
	}
}

bool UMWBotWeaponComponent::CanFire()
{
	const float time = GetWorld()->GetTimeSeconds();

	if (time > LastBurst + BurstPeriod)
	{
		ShotNumber = 0;
		LastBurst = time;
	}

	if (time > LastShot + FirePeriod && ShotNumber < BurstCount)
	{
		LastShot = time;
		++ShotNumber;

		return true;
	}
	
	return false;
}

void UMWBotWeaponComponent::ProcessHit(FHitResult Hit)
{
	if (Hit.bBlockingHit)
	{
		AMWCharacter* character = Cast<AMWCharacter>(Hit.GetActor());
		if (character)
		{
			character->Hurt(Strength);
		}
	}
}




