// K.A. Aliev

#include "MicroWave.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AMWPatController::AMWPatController(const FObjectInitializer& OI)
	: Super(OI)
{
	BlackboardComp = OI.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackBoardComp"));

	BrainComponent = BehaviorComp = OI.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));
}

void AMWPatController::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);

	AMWPat* Pat = Cast<AMWPat>(InPawn);

	// start behavior
	if (Pat && Pat->Behavior)
	{
		// how this blackboard component knows about the behavior tree this controller running?
		BlackboardComp->InitializeBlackboard(Pat->Behavior->BlackboardAsset);

		BehaviorComp->StartTree(*(Pat->Behavior));
		
	}
}

void AMWPatController::OnReachedWaypoint(AMWWaypoint* Waypoint)
{
	// ignore accidentally overlapped waypoints, if they are not the goal
	if (Waypoint == NextWaypoint)
	{
		LastReachedWaypoint = Waypoint;
	}

	/*StandsOnWaypoint = Waypoint;*/
}

AMWWaypoint* AMWPatController::FindNearestWaypoint(bool bSetAsNext)
{
	const AMWPat* pat = Cast<AMWPat>(GetPawn());
	if (!pat)
	{
		return nullptr;
	}

	float minDist = MAX_FLT;
	AMWWaypoint* nearestWp = nullptr;

	// get all the waypoints in the level
	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsOfClass(this, AMWWaypoint::StaticClass(), foundActors);
	
	if (!foundActors.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("No waypoints in the level"));
		return nullptr;
	}

	for (AActor* actor : foundActors)
	{
		const float dist = (pat->GetActorLocation() - actor->GetActorLocation()).SizeSquared();

		AMWWaypoint* waypoint = Cast<AMWWaypoint>(actor);
		const FString patTag = pat->WaypointTag;
		const FString wpTag = waypoint->Tag;
		
		// tags must match
		if (dist < minDist && patTag == wpTag)
		{
			minDist = dist;
			nearestWp = waypoint;
		}
	}

	if (!nearestWp)
	{
		UE_LOG(LogTemp, Error, TEXT("Pat %s couldn't find a waypoint with matching tag %s"), *pat->GetName(), *pat->WaypointTag);
		return nullptr;
	}

	// check whether we are already stand on that waypoint
	// normally waypoints stay far away from each other, so there is only one possible overlapping at a given time
	TArray<AActor*> overlaps;
	pat->GetOverlappingActors(overlaps, AMWWaypoint::StaticClass());
	if (overlaps.Num() && overlaps[0] == nearestWp)
	{
		check(nearestWp->Next)
		nearestWp = nearestWp->Next;
	}

	if (nearestWp && bSetAsNext)
	{
		SetNextWaypoint(nearestWp);
	}

	return nearestWp;
}

void AMWPatController::SetNextWaypoint(AMWWaypoint* InNextWaypoint)
{
	NextWaypoint = InNextWaypoint;
}

AMWWaypoint* AMWPatController::GetNextWaypoint() const
{
	return NextWaypoint;
}

bool AMWPatController::WaypointReached() const
{
	return NextWaypoint != 0 && NextWaypoint == LastReachedWaypoint;
}

class UBehaviorTreeComponent* AMWPatController::GetBehaviorTree() const
{
	return BehaviorComp;
}

