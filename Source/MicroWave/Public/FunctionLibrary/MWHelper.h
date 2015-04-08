// K.A. Aliev

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "MWHelper.generated.h"

/**
 * This class contains helper functions of various purpose.
 */
UCLASS()
class MICROWAVE_API UMWHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	* Projects the given point on the floor and returns its position in the world space.
	* @param WorldContextObject Pointer to an actor in the level.
	* @param Point Point to be projected.
	* @param Bias Vertical shift from the projection position (positive value - shift upwards).
	* @return Projection position in the world space.
	*/
	static FVector ProjectPointOnFloor(const UObject* WorldContextObject, const FVector& Point, const float Bias = 0.f);
};
