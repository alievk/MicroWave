// K.A. Aliev

#pragma once

#include "ProceduralMeshComponent.h"
#include "MWLightconeMeshComponent.generated.h"

/**
 * Procedural lightcone mesh used to visualize NPC vision cone.
 */
UCLASS(Meta=(BlueprintSpawnableComponent))
class UMWLightconeMeshComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:

	/** Default UObject constructor */
	UMWLightconeMeshComponent(const FObjectInitializer& OI);

	// Begin UObject Interface
	void PostInitProperties() override;
	// End UObject Interface

	// Begin UActorComponent Interface
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// End UActorComponent Interface

	/** Returns the opening angle (in rads) */
	UFUNCTION(BlueprintCallable, Category=Lightcone)
	float GetOpeningAngle() const;

	/** Returns eyes focal point based on the lightcone geometry (radii and distance) */
	UFUNCTION(BlueprintCallable, Category=Lightcone)
	FVector GetFocalPoint() const;

	/** Returts true if the Point is in the Lightcone angle (no LOS check). */
	UFUNCTION(BlueprintCallable, Category=Lightcone)
	bool PointInViewangle(const FVector& Point) const;

	/** Returts true if any segment of the Lightcone hits given Actor. */
	UFUNCTION(BlueprintCallable, Category=Lightcone)
	bool LightconeHitsActor(const AActor* Actor) const;

	/** Returts true if any segment of the Lightcone hits given Component. */
	UFUNCTION(BlueprintCallable, Category = Lightcone)
	bool LightconeHitsComponent(const UPrimitiveComponent* Component) const;

	/**
	* Changes material's color etc.
	* @param NewState - If true switch to alarm state, else to normal.
	*/
	UFUNCTION(BlueprintCallable, Category = Lightcone)
	void SetAlarm(bool NewState);

	/** Adjust light properties to fit the color,
	* cone angle and other properties of this lightcone. */
	void AdjustLight(class USpotLightComponent* Light);

public:

	/** Inner (smaller) radius of the lightcone */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Lightcone)
	float InnerRadius;

	/** Outer (larger) radius of the lightcone */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Lightcone)
	float OuterRadius;

	/** The height of the cone */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Lightcone)
	float Distance;

	/** Number of traverse mesh segments */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Lightcone)
	int32 MeshUSegments;

	/** Number of lengthwise mesh segments */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Lightcone)
	int32 MeshVSegments;

	/** U-coord tiling */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Lightcone)
	float UTiling;

	/** V-coord tiling */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Lightcone)
	float VTiling;

	/** Cut all the edges of the lightcone to the shortest edge */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Lightcone)
	uint8 bCutToShortestEdge : 1;

protected:

	/** Updates the mesh geometry */
	void UpdateMesh();

protected: // Properties

	/** Alarm state */
	UPROPERTY(Transient)
	uint8 bIsAlarm : 1;

	/** Each actor/component which was hit by the lighcone last frame */
	TArray<AActor*> HitActors;
	TArray<UPrimitiveComponent*> HitComponents;
};
