// K. A. Aliev

#pragma once

#include "MWBattery.generated.h"

/**
 * MW battery actor, used by MW gun.
 */
UCLASS(BlueprintType, Blueprintable)
class AMWBattery : public AActor
{
	GENERATED_BODY()

	/** How much LEDs we have on the battery */
	static const uint32 kMaxLEDs = 7;

public:

	/** Standard UObject constructor */
	AMWBattery(const FObjectInitializer&);

	/** Attach LED meshes and so on */
	void PostInitializeComponents() override;

	// Begin AActor Interface
	void Tick(float) override;
	// End AActor Interface

	/** Power-up */
	void OnPowerUpStart();
	void OnPowerUpEnd();

	/** Attaches this actor to its owning pawn */
	void OnEquip();

	/** Consume charge based on current power. Returns charge ammount actually consumed. */
	float ConsumeCharge(float DeltaTime);

	/** Restores charge by given value. */
	UFUNCTION(BlueprintCallable, Category=Battery)
	void RestoreCharge(float Charge);

	/** Set the owning pawn which equips this battery */
	void SetOwningPawn(class AMWCharacter* Pawn);

	/** Returns current charge */
	float GetCurrentCharge() const;

	/** Returns current power */
	float GetCurrentPower() const;

	/** Returns true if battery can has any charge */
	bool HasCharge() const;

private:

	/** Update material attributes according to CurrentCharge and CurrentPower vars */
	void UpdateLEDs();

public:

	/** Maximum power */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxPower;

	/** Normal power */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float NormalPower;

	/** Capacity */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Capacity;

	/** Power-up rate */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PowerUpRate;

protected:

	/** Battery mesh */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class USkeletalMeshComponent* BatteryMesh;

	/** LED mesh */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UStaticMesh* LEDMesh;

	/** LED mesh components array */
	class UStaticMeshComponent* LEDs[kMaxLEDs];

private:

	/** Pawn which owns this actor */
	UPROPERTY(Transient)
	class AMWCharacter* MyPawn;

	/** Current power */
	UPROPERTY(Transient)
	float CurrentPower;

	/** Current charge */
	UPROPERTY(Transient)
	float CurrentCharge;

	/** Wants power up */
	UPROPERTY(Transient)
	uint8 bWantsPowerUp : 1;

	/** MIDs of LEDs. They control "charge" and "power" illumination */
	UPROPERTY(Transient)
	class UMaterialInstanceDynamic* LED_MIDs[kMaxLEDs];

	/** MID of the battery mesh */
	UPROPERTY(Transient)
	class UMaterialInstanceDynamic* BatteryMID;
};
