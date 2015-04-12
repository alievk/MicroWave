// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "Level/CoverPathBuilder.h"

#include "MWCharacterMovement.generated.h"

/**
 * Movement component meant to be used with MicroWave characters.
 */
UCLASS()
class MICROWAVE_API UMWCharacterMovement : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	/** UObject constructor */
	UMWCharacterMovement(const FObjectInitializer& OI);

	// Begin UActorComponent Interface
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	// End UActorComponent Interface

	// Begin PawnMovementCompenent Interface
	//UFUNCTION(BlueprintCallable, Category = "MWCharacter|Components|Movement")
	//void AddInputVector(FVector WorldVector, bool bForce = false);
	// End PawnMovementCompenent Interface

	/** Immediatelly start taking the nearest cover point in the given radius
	* @param SearchRadius Radius of area, where to look for cover points
	* @param ReachTolerance How close we must reach the cover point to complete the movement.
	*/
	UFUNCTION(BlueprintCallable, Category = "MWCharacter|Components|Movement")
	void TakeNearestCoverPoint(const float SearchRadius = 500.f, const float ReachTolerance = 10.f);

	void StopTakingCover();

	/** Move the pawn along the cover path, if the owner pawn takes cover at the moment
	* @param AxisValue If this value > 0, then move rightwards. If < 0, then move leftwards. If = 0, no movement.
	*/
	UFUNCTION(BlueprintCallable, Category = "MWCharacter|Components|Movement")
	void MoveAlongCoverPath(const float AxisValue = 1.f);

	/** @return True, if taking cover at the moment */
	UFUNCTION(BlueprintCallable, Category = "MWCharacter|Components|Movement")
	bool IsTakingCover() const;

//public:
	/** */

private:
	/** Gives an order to move to the new cover point 
	* @param NewCoverPoint New goal.
	* @param bCompleteMovement If true, try to reach the goal; otherwise, do a single step towards the goal.
	*/
	void MoveToCoverPoint(const FCoverPoint& NewCoverPoint, bool bCompleteMovement = true);

	/** Continue movement to the cover point */
	void ContinueCoverTaking();

	/** Looks for the nearest cover point in the level
	* @param OutCoverPoint [out] Found cover point.
	* @param SearchRadius Radius of area, where to look for cover points
	* @return True, if any cover point is found.
	*/
	bool FindNearestCoverPoint(FCoverPoint& OutCoverPoint, const float SearchRadius);

	/** Looks for the next from the current cover point in the current cover path
	* @param OutCoverPoint [out] The next cover point in path in the given direction.
	* @param RightDirection If we look at the cover path and cover point indices grow from left to right, then if this param is true - take i+1-th cover point
	* @return True, if the call was successful (the pawn is taking cover at the moment).
	*/
	bool FindNextCoverPointInPath(FCoverPoint& OutCoverPoint, const bool bRightDirection);

	/** Returns distance indicating how close pawn's feet must be to a cover point to treat that cover point as reached. */
	float GetReachCoverPointTolerance();

	/** Returns pointer to the CoverPathBuilder instance */
	class ACoverPathBuilder* GetCoverPathBuilder();

private:
	/** If true, a movement to the target cover point is required */
	UPROPERTY()
	uint8 bReachedCoverPoint : 1;

	/** If true, always try to reach the target cover point */
	UPROPERTY()
	uint8 bWantsCompleteMovement : 1;

	/** True, when taking cover */
	UPROPERTY()
	uint8 bTakingCover : 1;

	/** Next cover point to take */
	FCoverPoint TargetCoverPoint;

	/** Next cover point to take */
	FCoverPoint CurrentCoverPoint;

	/** Cached pointer to the CoverPathBuilder */
	UPROPERTY()
	class ACoverPathBuilder* CoverPathBuilder;
};
