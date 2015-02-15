

#pragma once

#include "GameFramework/Character.h"
#include "MWMot.generated.h"

/**
 * Mounted guard character class.
 */
UCLASS()
class MICROWAVE_API AMWMot : public APawn, public IInterface_MechanicBot
{
	GENERATED_BODY()

public:
	/** Default UObject constructor. */
	AMWMot(const FObjectInitializer& OI);

	/** Finalize the pat setup */
	void PostInitializeComponents() override;

	// Begin AActor Interface
	void Tick(float DeltaSeconds) override;
	// Begin AActor Interface

	// Begin MechanicBot Interface
	bool TakeMicrowave(float Power) override;
	const class UPrimitiveComponent* GetShape() const override;
	void OnCorruptedStart();
	void OnCorruptedEnd();
	void OnKaput();
	// End MechanicBot Interface

	/** Try to aim mot at the given actor | point
	* @param Actor - An actor to aim at.
	* @param Point - If Actor is nullptr, this is the point to aim at.
	* @param Tolerance - Angle in degrees between the target and the mot's face direction when mot considered to be aimed at that target.
	* @return True, if successfuly aimed.
	*/
	UFUNCTION(Category = Mot, BlueprintCallable)
	bool LookAt(const AActor* Actor, const FVector Point, const float Tolerance);

	/** Guard a given corner (while calling this function) */
	UFUNCTION(Category=Mot, BlueprintCallable)
	void Guard();

	/*
	* Affects parameters depending on the alarm level.
	* @param DangerLevel - level of danger [0;1]
	*/
	UFUNCTION(Category = Mot, BlueprintCallable)
	void SetAlarm(float DangerLevel);

	/** Makes to go from alarm to guard mode */
	UFUNCTION(Category = Mot, BlueprintCallable)
	void Relax();

	/** Fire */
	UFUNCTION(Category = Mot, BlueprintCallable)
	void Fire();

public:

	/** Behaviour tree asset which controls this pat (containt the graph and blackboard asset) */
	UPROPERTY(Category = AI, EditAnywhere)
	class UBehaviorTree* Behavior;

	/** Aiming rotation (for Anim Blueprint) */
	UPROPERTY(BlueprintReadOnly, Category = Animation)
	FRotator AimRotation;

	/** How fast mot changes its aiming orientation in the guard state. If 0, reacts immediatelly. */
	UPROPERTY(Category = BotSensor, EditDefaultsOnly)
	float GuardReaction;

	/** Guard corner lower angle limit. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator GuardAngleLower;

	/** Guard corner upper angle limit. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator GuardAngleUpper;

	/** Guarding point changing period, sec. */
	UPROPERTY(EditDefaultsOnly, Category = Mot)
	float GuardPointChangePeriod;

	/** How fast mot changes its aiming orientation in the alarm state. If 0, reacts immediatelly. */
	UPROPERTY(Category = BotSensor, EditDefaultsOnly)
	float AlarmReaction;

	/** Level of danger when this mot enters the alarm state [0; 1] */
	UPROPERTY(Category = AI, EditDefaultsOnly, BlueprintReadOnly)
	float AlarmLevel;

protected:

	/** Dummy scene component to constraint the Mesh to */
	//UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	//class USceneComponent* Base;

	/** This component mounts the Mesh to Base */
	/*UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class UPhysicsConstraintComponent* MeshConstraint;*/

	/** Skeletal mesh */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class USkeletalMeshComponent* Mesh;

	/** Weapon */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class UMWBotWeaponComponent* Weapon;

	/** Meash socket name for the weapon */
	UPROPERTY(Category=Weapon, EditDefaultsOnly, BlueprintReadOnly)
	FName WeaponSocket;

	/** The volume which consumes MW radiation */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class UCapsuleComponent* Shape;

	/** Mot's "eye" */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class UMWBotSensorComponent* Sensor;

	/** Mesh socket where the sensor attached to */
	UPROPERTY(Category=Sensor, EditDefaultsOnly, BlueprintReadOnly)
	FName SensorSocket;

	/** Light cone, attached to the sensor */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class UMWLightconeMeshComponent* Lightcone;

	/** Lightcone material */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UMaterialInterface* LightconeMaterial;

	/** Spot light component */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class USpotLightComponent* SpotLight;

	/** Health component */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class UMWBotHealthComponent* HealthComponent;

private:

	/** Smoothly update the rotation and clamp the angle if necessary*/
	void UpdateRotation(float DeltaSeconds);
		
private:

	/** Desired aim rotation */
	UPROPERTY(Transient)
	FRotator DesiredAimRotation;

	/** Time when last wandered */
	float LastGuardPointChange;

	/** True when corrupted */
	UPROPERTY(Transient)
	uint8 bCrazy : 1;
};
