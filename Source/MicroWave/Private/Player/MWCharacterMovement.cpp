// Fill out your copyright notice in the Description page of Project Settings.

#include "MicroWave.h"
#include "Player/MWCharacter.h"
#include "Player/MWCharacterMovement.h"
#include "Level/CoverPathBuilder.h"

UMWCharacterMovement::UMWCharacterMovement(const FObjectInitializer& OI)
	: Super(OI)
{

}

void UMWCharacterMovement::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bReachedCoverPoint)
	{
		ContinueCoverTaking();

		if (!bWantsCompleteMovement)
		{
			bReachedCoverPoint = true;
		}
	}
}

void UMWCharacterMovement::TakeNearestCoverPoint(const float SearchRadius, const float ReachTolerance)
{
	FCoverPoint CoverPoint;
	const bool bFound = FindNearestCoverPoint(CoverPoint, SearchRadius);

	if (bFound)
	{
		MoveToCoverPoint(CoverPoint);
	}
}

void UMWCharacterMovement::MoveAlongCoverPath(const float AxisValue)
{
	if (bTakingCover && AxisValue != 0.f)
	{
		const bool RightDir = AxisValue > 0.f;
		FCoverPoint NextCoverPoint;
		
		if (FindNextCoverPointInPath(NextCoverPoint, RightDir))
		{
			MoveToCoverPoint(NextCoverPoint, false);
		}
	}
}

bool UMWCharacterMovement::IsTakingCover() const
{
	return bTakingCover; 
}

void UMWCharacterMovement::MoveToCoverPoint(const FCoverPoint& NewCoverPoint, bool bCompleteMovement)
{
	TargetCoverPoint = NewCoverPoint;
	bReachedCoverPoint = false;
	bWantsCompleteMovement = bCompleteMovement;
}

void UMWCharacterMovement::ContinueCoverTaking()
{
	AMWCharacter* MyPawn = Cast<AMWCharacter>(GetOwner());

	if (MyPawn && !bReachedCoverPoint)
	{
		const FVector DistanceVector = TargetCoverPoint.Location - GetActorFeetLocation();

		if (DistanceVector.Size() < GetReachCoverPointTolerance())
		{
			bReachedCoverPoint = true;
			bTakingCover = true;
			CurrentCoverPoint = TargetCoverPoint;
		}
		else
		{
			// Simple move to the goal
			const FVector MoveDir = DistanceVector.GetSafeNormal();
			AddInputVector(MoveDir);

			// do auto-crouch here
		}
	}
}

bool UMWCharacterMovement::FindNearestCoverPoint(FCoverPoint& OutCoverPoint, const float SearchRadius)
{
	if (GetCoverPathBuilder())
	{
		return GetCoverPathBuilder()->FindNearestCoverPoint(OutCoverPoint, GetCharacterOwner(), SearchRadius);
	}

	return false;
}

bool UMWCharacterMovement::FindNextCoverPointInPath(FCoverPoint& OutCoverPoint, const bool bRightDirection)
{
	if (!bTakingCover || !GetCoverPathBuilder())
	{
		return false;
	}

	// Get the cover path the current cover point belongs to
	FCoverPath CurCoverPath;
	if (!GetCoverPathBuilder()->GetLevelCoverPath(CurCoverPath, CurrentCoverPoint.PathIdx))
	{
		return false;
	}

	const int32 LastCoverPointIdx = CurCoverPath.CoverPoints.Last().CoverPointIdx;
	
	const int32 InvalidIdx = -1;
	int32 NextCoverPointIdx = InvalidIdx;

	// For left-sided path, cover point indices increase when following the path from left to right
	if (EWallHand::EWO_Left == CurCoverPath.WallHand)
	{
		if (bRightDirection)
		{
			if (LastCoverPointIdx == CurrentCoverPoint.CoverPointIdx)
			{
				// reached the last cover point in the path
				NextCoverPointIdx = CurrentCoverPoint.CoverPointIdx;
			}
			else
			{
				NextCoverPointIdx = CurrentCoverPoint.CoverPointIdx + 1;
			}
		}
		else
		{
			if (0 == CurrentCoverPoint.CoverPointIdx)
			{
				// reached the last cover point in the path
				NextCoverPointIdx = CurrentCoverPoint.CoverPointIdx;
			}
			else
			{
				NextCoverPointIdx = CurrentCoverPoint.CoverPointIdx - 1;
			}
		}
	}
	// For right-sided path, cover point indices increase when following the path from right to left
	else if (EWallHand::EWO_Right == CurCoverPath.WallHand)
	{
		if (bRightDirection)
		{
			if (0 == CurrentCoverPoint.CoverPointIdx)
			{
				// reached the last cover point in the path
				NextCoverPointIdx = CurrentCoverPoint.CoverPointIdx;
			}
			else
			{
				NextCoverPointIdx = CurrentCoverPoint.CoverPointIdx - 1;
			}
		}
		else
		{
			if (LastCoverPointIdx == CurrentCoverPoint.CoverPointIdx)
			{
				// reached the last cover point in the path
				NextCoverPointIdx = CurrentCoverPoint.CoverPointIdx;
			}
			else
			{
				NextCoverPointIdx = CurrentCoverPoint.CoverPointIdx + 1;
			}
		}
	}

	if (InvalidIdx != NextCoverPointIdx)
	{
		OutCoverPoint = CurCoverPath.CoverPoints[NextCoverPointIdx];

		return true;
	}

	return false;
}

float UMWCharacterMovement::GetReachCoverPointTolerance()
{
	if (GetCoverPathBuilder())
	{
		// PathStep is the distance between two cover points
		return GetCoverPathBuilder()->PathStep / 2.f;
	}

	return 10.f; // fallback value
}

ACoverPathBuilder* UMWCharacterMovement::GetCoverPathBuilder()
{
	if (CoverPathBuilder)
	{
		return CoverPathBuilder; // return cached pointer
	}

	// @TODO FIX THIS BEHAVIOUR!!!
	// Below, look for at least one instance of ACoverPathBuilder in the level and cache it.
	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACoverPathBuilder::StaticClass(), AllActors);

	if (AllActors.Num() > 0)
	{
		return CoverPathBuilder = CastChecked<ACoverPathBuilder>(AllActors[0]);
	}

	return nullptr;
}

