// K.A. Aliev

#include "MicroWave.h"
#include "MWLibrary.h"

TArray<FHitResult> UMWLibrary::CanTakeCover(
	const FVector& Location,
	const FVector& Direction,
	const FVector& SearchArea,
	const float Accuracy,
	const float AgentRadius)
{
	TArray<FHitResult> HitResults;

	if (GetWorld())
	{
		FVector TraceStart;
		FVector TraceEnd;
		const ECollisionChannel TraceChannel = ECollisionChannel::ECC_WorldStatic;
		const FCollisionQueryParams TraceParams;
		FHitResult Hit(ForceInit);

		GetWorld()->LineTraceSingle(Hit, TraceStart, TraceEnd, TraceChannel, TraceParams);
	}

	return HitResults;
}