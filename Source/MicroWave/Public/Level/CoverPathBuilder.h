// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Level/WallDenotation.h"
#include "CoverPathBuilder.generated.h"

/**
* This structure is a result of a cover path build and holds information about a singular cover point, such as location and maximum safe (agent's) head distance from the floor.
*/
USTRUCT()
struct FCoverPoint
{
	GENERATED_USTRUCT_BODY()

	/** Location of the cover point, projected on the floor. */
	UPROPERTY()
	FVector Location;

	/** Maximum safe head distance from the floor (actually, from the Location). */
	UPROPERTY()
	float MaxSafeHeight;

	FCoverPoint()
	{
	}
};

/**
* Contains cover path info, such as cover points and the wall orientation.
*/
USTRUCT()
struct FCoverPath
{
	GENERATED_USTRUCT_BODY()

	/** Chain of cover points. */
	UPROPERTY()
	TArray<FCoverPoint> CoverPoints;

	/** Indicates where the wall is situated, when bypassing cover points, from the first to the last. */
	UPROPERTY()
	TEnumAsByte<EWallHand> WallHand;

	FCoverPath()
	{
	}
};

UCLASS()
class MICROWAVE_API ACoverPathBuilder : public AActor
{
	GENERATED_BODY()
	
public:	
	ACoverPathBuilder(const FObjectInitializer& OI);

	// Begin AActor Interface
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
	// End AActor Interface

protected:
	/**
	* Examine the wall at the given point and achieve cover point info.
	* @param OutCoverPoint [out] Contains valid cover point info, if at this point and direction cover could be token.
	* @param FootPoint Point, where max safe head height search starts.
	* @param Direction Direction from FootPoint, where a wall is situated.
	* @return True, if at this point cover could be taken.
	*/
	bool FindCoverPoint(FCoverPoint& OutCoverPoint, const FVector& FootPoint, const FVector& Direction) const;

	/**
	* Find a cover path between two corners.
	* @param OutCoverPath [out] Cover path found between the corners.
	* @param FirstCorner First corner position.
	* @param SecondCorner Second corner position.
	* @param WallHand Defines where to look for a wall when stepping from the first to the second corner.
	* @return True, if the path is found.
	*/
	bool FindSimpleCoverPath(FCoverPath& OutCoverPath, const FVector& FirstCorner, const FVector& SecondCorner, EWallHand WallHand = EWallHand::EWO_Right) const;

	/**
	* Find a cover path between all the corners of the given wall.
	* @param OutCoverPaths [out] Array of cover points found along the wall.
	* @param Wall Pointer to a wall denotation actor.
	*/
	void FindCoverPathsAlongWall(TArray<FCoverPath>& OutCoverPaths, const AWallDenotation* Wall) const;

	/** Try to find paths between all the corners in the level. */
	void FindAllPaths();

	/** Draw cover points in the level */
	void DrawCoverPoints() const;

protected:

	/** Root component */
	//UPROPERTY()
	//USceneComponent* SceneComponent;

	/** Agent height in standing position. */
	UPROPERTY(EditAnywhere)
	float AgentMaxHeight;

	/** Minimum possible agent height (when lying prone or crouching). */
	UPROPERTY(EditAnywhere)
	float AgentMinHeight;

	/** Radius of tha agent. Affects the offset of the cover point from the wall. */
	UPROPERTY(EditAnywhere)
	float AgentRadius;

	/** Vertical distance between two raycasts, affects how accurately the max safe height of the wall is examined. */
	UPROPERTY(EditAnywhere)
	float SafeHeightAccuracy;

	/** Distance between two cover points. */
	UPROPERTY(EditAnywhere)
	float PathStep;

	/** Offset of the cover points from the floor */
	UPROPERTY(EditAnywhere)
	float FloorOffset;

	/** If true, draw cover points in the level */
	UPROPERTY(EditAnywhere)
	uint8 bDrawCoverPoints : 1;

private:

	/** This value, when false, triggers cover paths build. */
	uint8 bHasBuiltPaths : 1;
	
	/** Cover paths found in the level. */
	TArray<FCoverPath> LevelCoverPath;
};
