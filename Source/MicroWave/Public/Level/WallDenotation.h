// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "WallDenotation.generated.h"

/**
* Indication of where wall is situated when bypassing corners.
*/
UENUM(BlueprintType)
enum EWallHand
{
	EWO_Left UMETA(DisplayName="Left"),
	EWO_Right UMETA(DisplayName="Right")
};

/**
* Fill description
*/
UCLASS()
class MICROWAVE_API AWallDenotation : public AActor
{
	GENERATED_BODY()

public:
	AWallDenotation(const FObjectInitializer& OI);

	//virtual void BeginPlay() override;

#if WITH_EDITOR
	// Begin AActor Interface
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	// End AActor Interface
#endif // WITH_EDITOR

	/** Returns corner location in the world */
	FVector GetCornerWorldLocation(int32 CornerIndex) const;

public:
	/** First corner position (widget) */
	UPROPERTY(EditAnywhere, Category = Corners, meta = (MakeEditWidget = "Corner"))
	TArray<FVector> Corners;

	/** Where wall is situated when bypassing the corners, from the first to the last. */
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EWallHand> WallHand;

	/** If true, close the corners chain connecting the last corner with the first corner */
	UPROPERTY(EditAnywhere)
	uint8 bClosed:1;

#if WITH_EDITOR
	/** Wall denotation icon */
	UPROPERTY(VisibleDefaultsOnly)
	class UBillboardComponent* SpriteComponent;
#endif

private:
	/** Call this function when one of the corner positions is moved, it updates the floor projections of the corner positions. */
	void OnCornerMoved();
};
