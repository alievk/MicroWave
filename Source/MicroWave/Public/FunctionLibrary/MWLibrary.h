// K.A. Aliev

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "MWLibrary.generated.h"

/**
 * 
 */
UCLASS()
class MICROWAVE_API UMWLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

		/** Description needed
		* @param Location - Point (in the world space) where we look for a cover
		* @param Direction - Direction (in world space) in which we look for a cover from the Point
		* @param SearchArea - Defines a 3D box in which we look for a cover
		* @param Accuracy - This value defines how dense raycasts are, in [cm]. For example, if SearchArea.X is 100 and Accuracy is 10, there will be 10 raycasts along X-axis.
		* @param AgentRadius - Radius of the player character, in [cm]. Effectively, this is the horisontal offset from the cover (if it's found).
		*/
		UFUNCTION(BlueprintCallable, Category = CoverSystem)
		TArray<FHitResult> CanTakeCover(
			const FVector& Location,
			const FVector& Direction,
			const FVector& SearchArea,
			const float Accuracy,
			const float AgentRadius);
	
};
