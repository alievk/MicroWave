// Fill out your copyright notice in the Description page of Project Settings.

#include "MicroWave.h"
#include "CoverPathBuilder.h"
#include "Level/WallDenotation.h"
#include "FunctionLibrary/MWHelper.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ACoverPathBuilder::ACoverPathBuilder(const FObjectInitializer& OI)
{
	USceneComponent* SceneComponent = OI.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootComponent"));
	RootComponent = SceneComponent;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetActorLocation(FVector::ZeroVector);

	AgentMaxHeight = 180;
	AgentMinHeight = 90;
	AgentRadius = 40;
	SafeHeightAccuracy = 10;
	PathStep = 50;
	FloorOffset = 5;
	bDrawCoverPoints = false;

	bHasBuiltPaths = false;
}

// Called when the game starts or when spawned
void ACoverPathBuilder::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACoverPathBuilder::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	// We do it here and not in BeginPlay because we want to make sure all corner denotion actors are spawned and the paths are built in simulation mode.
	if (!bHasBuiltPaths)
	{
		FindAllPaths();
		bHasBuiltPaths = true;
	}

	if (bDrawCoverPoints)
	{
		DrawCoverPoints();
	}
}

bool ACoverPathBuilder::FindNearestCoverPoint(FCoverPoint& OutCoverPoint, ACharacter* Pawn, const float SearchRadius) const
{
	check(Pawn)

	// Find all the cover points in the given distance from the pawn
	const FVector PawnFeetLocation = Pawn->GetMovementComponent()->GetActorFeetLocation();
	TArray<FCoverPoint> CoverPointsNearby;
	for (FCoverPath CoverPath : LevelCoverPath)
	{
		for (FCoverPoint CoverPoint : CoverPath.CoverPoints)
		{
			const float Dist = FVector::Dist(PawnFeetLocation, CoverPoint.Location);
			if (Dist < SearchRadius)
			{
				CoverPointsNearby.Add(CoverPoint);
			}
		}
	}

	// Find all the cover points which could be reached by a straight movement
	const FVector TraceStart = Pawn->GetActorLocation();
	const UCapsuleComponent* PawnCapsule = Pawn->GetCapsuleComponent();
	//TArray<AActor*> DummyArray;
	TArray<FCoverPoint> ReachableCoverPoints;
	for (FCoverPoint CoverPoint : CoverPointsNearby)
	{
		// Sweep pawn's capsule from the pawn to the cover point and see if there any obstacle
		const FVector TraceEnd = CoverPoint.Location + FVector(0.f, 0.f, PawnCapsule->GetUnscaledCapsuleHalfHeight());
		FHitResult HitResult;
		UKismetSystemLibrary::CapsuleTraceSingle_NEW(
			Pawn, 
			TraceStart, 
			TraceEnd, 
			PawnCapsule->GetUnscaledCapsuleRadius(), 
			PawnCapsule->GetUnscaledCapsuleHalfHeight(), 
			ETraceTypeQuery::TraceTypeQuery1, // Visibility, Camera
			false, 
			TArray<AActor*>(), 
			EDrawDebugTrace::None, 
			HitResult,
			true);

		if (!HitResult.bBlockingHit)
		{
			ReachableCoverPoints.Add(CoverPoint);
		}
	}

	// Find the nearest cover point
	float NearestDistance = SearchRadius;
	int32 iNearestCoverPoint = -1; // this means no cover point was found
	for (int32 i = 0; i < ReachableCoverPoints.Num(); ++i)
	{
		const float Dist = FVector::Dist(PawnFeetLocation, ReachableCoverPoints[i].Location);
		if (Dist < NearestDistance)
		{
			iNearestCoverPoint = i;
			NearestDistance = Dist;
		}
	}

	if (-1 != iNearestCoverPoint)
	{
		OutCoverPoint = ReachableCoverPoints[iNearestCoverPoint];
		return true;
	}

	return false;
}

bool ACoverPathBuilder::GetLevelCoverPath(FCoverPath& OutCoverPath, const int32 PathIdx) const
{
	if (PathIdx >= 0 && PathIdx < LevelCoverPath.Num())
	{
		if (PathIdx == LevelCoverPath[PathIdx].PathIdx)
		{
			OutCoverPath = LevelCoverPath[PathIdx];

			return true;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("PathIdx argument is not equal to the PathIdx of the cover path it referencing to"))
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PathIdx out of range"))
	}

	return false;
}

bool ACoverPathBuilder::FindCoverPoint(FCoverPoint& OutCoverPoint, const FVector& FootPoint, const FVector& Direction) const
{
	if (!GetWorld())
	{
		return false;
	}

	const int CastNum = AgentMaxHeight / SafeHeightAccuracy; // !!! THIS IS UNSAFE
	const float CastDist = 4.f * AgentRadius; // this length would be enough
	const FVector Up(0.f, 0.f, 1.f);

	// The following loop casts a series of rays to examine the max height from the floor, where there are no 'holes' in the wall
	float MaxSafeHeight = 0.f;
	float NearestImpactDist = CastDist;
	bool bHitFail = false;
	for (int i = 0; i < CastNum, !bHitFail; ++i)
	{
		const float VOffset = i * SafeHeightAccuracy;
		const FVector VOffsetVector = Up * VOffset;
		const FVector TraceStart = FootPoint + VOffsetVector;
		const FVector TraceEnd = TraceStart + CastDist * Direction;

		static const FName TraceTag = FName(TEXT("FindCoverPointTrace"));
		const FCollisionQueryParams TraceParams(TraceTag);
		FHitResult HitResult(ForceInit);

		GetWorld()->LineTraceSingle(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, TraceParams);

		// If have a blocking hit, remember the height from the floor and the length of the trace line from the beginning to the hit point
		if (HitResult.bBlockingHit)
		{
			const float ImpactPointDist = FVector::Dist(HitResult.ImpactPoint, TraceStart);
			if (ImpactPointDist < NearestImpactDist)
			{
				NearestImpactDist = ImpactPointDist;
			}

			MaxSafeHeight = VOffset;
		}
		else
		{
			bHitFail = true;
		}
	}

	// Return success if only the wall's safe height is not less than the min height the agent can have
	if (MaxSafeHeight > AgentMinHeight)
	{
		// Fill out the CoverPoint struct
		const float OffsetFromFoot = NearestImpactDist - AgentRadius;
		OutCoverPoint.Location = FootPoint + Direction * OffsetFromFoot;
		OutCoverPoint.MaxSafeHeight = MaxSafeHeight;

		return true;
	}

	return false;
}

bool ACoverPathBuilder::FindSimpleCoverPath(FCoverPath& OutCoverPath, const FVector& FirstCorner, const FVector& SecondCorner, EWallHand WallHand) const
{
	const FVector PathStart = UMWHelper::ProjectPointOnFloor(this, FirstCorner, FloorOffset);
	const FVector PathEnd = UMWHelper::ProjectPointOnFloor(this, SecondCorner, FloorOffset);
	
	const FVector Forward = (PathEnd - PathStart).GetSafeNormal();
	const FVector Up(0.f, 0.f, 1.f);
	const FVector Right = Up ^ Forward;

	const float Distance = FVector::Dist(PathStart, PathEnd);

	OutCoverPath.WallHand = WallHand;
	OutCoverPath.CoverPoints.Empty();

	float CurrentCoord = 0.f; // Forward axis coordinate
	bool bBroken = false; // true, if the path breakes
	bool bPathStarted = false; // becames true, when the first cover point found

	// Try to build a cover path between the corners
	while (CurrentCoord < Distance && !bBroken)
	{
		const FVector FootPoint = PathStart + CurrentCoord * Forward;
		bool bCoverPointFound = false;
		FCoverPoint CoverPoint;

		switch (WallHand)
		{
		case EWallHand::EWO_Left:
			bCoverPointFound = FindCoverPoint(CoverPoint, FootPoint, -Right);
			break;
		case EWallHand::EWO_Right:
			bCoverPointFound = FindCoverPoint(CoverPoint, FootPoint, Right);
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("Invalid WallHand value"));
		}

		if (bCoverPointFound)
		{
			OutCoverPath.CoverPoints.Add(CoverPoint);

			bPathStarted = true;
		}
		else if (bPathStarted)
		{
			// abort the loop if the path was started and then broken
			bBroken = true;

			UE_LOG(LogTemp, Warning, TEXT("Path has been broken"));
		}

		// Move along Forward axis
		CurrentCoord += PathStep;
	}

	return bPathStarted && !bBroken;
}

void ACoverPathBuilder::FindCoverPathsAlongWall(TArray<FCoverPath>& OutCoverPaths, const AWallDenotation* Wall) const
{
	if (Wall && Wall->Corners.Num() > 1)
	{
		// Find paths between all the corners of the wall
		for (int32 i = 0; i <= Wall->Corners.Num(); ++i)
		{
			const int32 iLastCorner = Wall->Corners.Num() - 1;
			const int32 iInvalidCorner = -1;
			int32 iCorner1 = iInvalidCorner;
			int32 iCorner2 = iInvalidCorner;

			if (i < iLastCorner)
			{
				iCorner1 = i;
				iCorner2 = i + 1;
			}
			else if (i > 2 && Wall->bClosed)
			{
				// Connect the last and the first corners, if there are enough corners (>2) and bClosed is true.
				iCorner1 = iLastCorner;
				iCorner2 = 0;
			}

			if (iInvalidCorner != iCorner1 && iInvalidCorner != iCorner2)
			{
				const FVector Corner1 = Wall->GetCornerWorldLocation(iCorner1);
				const FVector Corner2 = Wall->GetCornerWorldLocation(iCorner2);

				// Find and add a cover path
				FCoverPath CoverPath;
				const bool PathFound = FindSimpleCoverPath(CoverPath, Corner1, Corner2, Wall->WallHand);

				if (PathFound)
				{
					const int32 PathIdx = OutCoverPaths.Num();
					CoverPath.Init(PathIdx); // assign path index
					OutCoverPaths.Add(CoverPath);
				}
			}
		} // for all corners
	} // if wall is valid
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid wall denotation instance"));
	}

	return ;
}

void ACoverPathBuilder::FindAllPaths()
{
	if (!GetWorld())
	{
		return;
	}

	TArray<AActor*> AllWalls;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWallDenotation::StaticClass(), AllWalls);

	LevelCoverPath.Empty();

	for (AActor* Actor : AllWalls)
	{
		const AWallDenotation* Wall = Cast<AWallDenotation>(Actor);
		if (Wall)
		{
			// A wall denotation can consist of several cover paths, so we have to save all
			TArray<FCoverPath> CoverPaths;
			FindCoverPathsAlongWall(CoverPaths, Wall);

			for (int i = 0; i < CoverPaths.Num(); ++i)
			{
				LevelCoverPath.Add(CoverPaths[i]);
			}
		}
	}
}

void ACoverPathBuilder::DrawCoverPoints() const
{
	if (!GetWorld())
	{
		return;
	}

	// Draw little spheres at the cover points, with vertical offset equal to the safe wall height
	for (int i = 0; i < LevelCoverPath.Num(); ++i)
	{
		FCoverPath CoverPath = LevelCoverPath[i];
		for (int j = 0; j < CoverPath.CoverPoints.Num(); ++j)
		{
			const FCoverPoint CoverPoint = CoverPath.CoverPoints[j];
			const FVector Up(0.f, 0.f, 1.f);
			const FVector Pos = CoverPoint.Location + Up * CoverPoint.MaxSafeHeight;
			const float Radius = 5.f;
			const int32 Segments = 8;
			const FColor Color = FColor::Red;
			DrawDebugSphere(GetWorld(), Pos, Radius, Segments, Color, false);
		}
	}
}