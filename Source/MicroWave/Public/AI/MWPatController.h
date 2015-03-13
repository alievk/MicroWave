// K.A. Aliev

#pragma once

#include "AIController.h"
#include "MWPatController.generated.h"

class AMWWaypoint;

UCLASS()
class MICROWAVE_API AMWPatController : public AAIController
{
	GENERATED_BODY()

public:

	/** Standard constructor */
	AMWPatController(const FObjectInitializer&);

	// Begin AController interface
	void Possess(class APawn* InPawn) override;
	// End APlayerController interface

	/**
	* Invoked when this pat reaches (overlaps a shape of) a waypoint
	* @param Waypoint - Reached waypoint.
	*/
	void OnReachedWaypoint(AMWWaypoint* Waypoint);

	/**
	* Returns nearest waypoint to this pat 
	* @param bSetAsNext - If true, set as the next waypoint
	*/
	UFUNCTION(BlueprintCallable, Category = AI)
	AMWWaypoint* FindNearestWaypoint(bool bSetAsNext);

	/** Set the waypoint this pat has to go to */
	UFUNCTION(BlueprintCallable, Category = AI)
	void SetNextWaypoint(AMWWaypoint* InNextWaypoint);

	/** Get the waypoint pat goes to */
	UFUNCTION(BlueprintCallable, Category = AI)
	AMWWaypoint* GetNextWaypoint() const;

	/** Returns true if the waypoint reached (NextWayoint == LastReachedWaypoint) */
	UFUNCTION(BlueprintCallable, Category = AI)
	bool WaypointReached() const;

	/** Returns the behavior tree this controller runs */
	UFUNCTION(BlueprintCallable, Category = AI)
	class UBehaviorTreeComponent* GetBehaviorTree() const;

protected:

	/** This guy runs the behavior graph of the posessed pawn */
	UPROPERTY(Transient)
	class UBehaviorTreeComponent* BehaviorComp;

	/** Instance of a blackboard for this controller */
	UPROPERTY(Transient)
	class UBlackboardComponent* BlackboardComp;
	
	/** Last waypoint this pat has reached intentionally */
	UPROPERTY(Transient)
	AMWWaypoint* LastReachedWaypoint;

	/** Next waypoint this pat wants to reach */
	UPROPERTY(Transient)
	AMWWaypoint* NextWaypoint;
};

