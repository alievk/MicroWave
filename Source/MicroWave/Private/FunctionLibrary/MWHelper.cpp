// K.A. Aliev

#include "MicroWave.h"
#include "MWHelper.h"

FVector UMWHelper::ProjectPointOnFloor(const UObject* WorldContextObject, const FVector& Point, const float Bias)
{
	FVector Proj; // result

	if (WorldContextObject && WorldContextObject->GetWorld())
	{
		const float RayLength = 1000000; // 10km
		const FVector TraceEnd = Point + FVector(0.f, 0.f, -1.f) * RayLength;
		static const FName TraceTag = FName(TEXT("FloorProjectionTrace"));
		const FCollisionQueryParams TraceParams(TraceTag);
		FHitResult HitResult(ForceInit);

		WorldContextObject->GetWorld()->LineTraceSingle(HitResult, Point, TraceEnd, ECollisionChannel::ECC_Visibility, TraceParams);

		if (HitResult.bBlockingHit)
		{
			Proj = HitResult.ImpactPoint + FVector(0.f, 0.f, Bias);
		}
		else
		{
			Proj = TraceEnd;
		}
	}

	return Proj;
}