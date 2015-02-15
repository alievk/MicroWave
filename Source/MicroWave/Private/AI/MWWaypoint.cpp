// K.A. Aliev

#include "MicroWave.h"

AMWWaypoint::AMWWaypoint(const FObjectInitializer& OI)
	: Super(OI)
{
	// Setup the waypoint sensor
	Sensor = OI.CreateDefaultSubobject<USphereComponent>(this, TEXT("WaypointSensor"));
	Sensor->AttachParent = RootComponent;
	Sensor->InitSphereRadius(80);
	Sensor->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sensor->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	Sensor->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Sensor->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Overlap);

}

void AMWWaypoint::ReceiveActorBeginOverlap(class AActor* Other)
{
	AMWPat* pat = Cast<AMWPat>(Other); // consider changing this to casting to more generic bot class

	// if touched a pat, notice its controller
	if (pat)
	{
		AMWPatController* patCtr = Cast<AMWPatController>(pat->GetController());

		if (patCtr)
		{
			patCtr->OnReachedWaypoint(this);
		}
	}
}