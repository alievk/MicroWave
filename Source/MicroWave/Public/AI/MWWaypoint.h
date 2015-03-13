// K.A. Aliev

#pragma once

#include "Engine/TargetPoint.h"
#include "MWWaypoint.generated.h"

/**
 * Patrolling waypoint class.
 */
UCLASS()
class MICROWAVE_API AMWWaypoint : public ATargetPoint
{
	GENERATED_BODY()

public:

	/** UObject constructor */
	AMWWaypoint(const FObjectInitializer&);

	// Begin AActor Interface
	void ReceiveActorBeginOverlap(class AActor* Other) override;
	// End AActor Interface

public:

	/** The next waypoint in the patrolling path */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waypoint)
	AMWWaypoint* Next;

	/** Name of this waypoint */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waypoint)
	FString Name;

	/** Tag of this waypoint (to let pats distinguish waypoints) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waypoint)
	FString Tag;
	
protected:

	/** "Sensitive" shape of the waypoint, which generate overlap events */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class USphereComponent* Sensor;
};
