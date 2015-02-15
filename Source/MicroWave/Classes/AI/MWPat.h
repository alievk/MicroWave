

#pragma once

#include "GameFramework/Character.h"
#include "MWPat.generated.h"

/**
 * Patrolling guard character class.
 */
UCLASS()
class MICROWAVE_API AMWPat : public ACharacter, public IInterface_MechanicBot
{
	GENERATED_BODY()
	
public:
	/** Default UObject constructor. */
	AMWPat(const FObjectInitializer& OI);

	/** Finalize the pat setup */
	void PostInitializeComponents() override;

	// Begin AActor Interface
	void Tick(float DeltaSeconds) override;
	// Begin AActor Interface

	/** Try to target pat's eye at the given actor */
	UFUNCTION(Category=Character, BlueprintCallable)
	void LookAt(const AActor* Actor, const FVector Point);

	/** Apply tilt to the mesh according to the alcceleration */
	UFUNCTION(Category=Character, BlueprintCallable)
	void ApplyTilt(float DeltaTime);

	/** Simple floating in air immitation */
	void Float();

	/** Goes to normal state */
	UFUNCTION(Category = Character, BlueprintCallable)
	void Relax();

	/*
	* Affects parameters depending on the alarm level.
	* @param DangerLevel - level of danger [0;1]
	*/
	UFUNCTION(Category=Character, BlueprintCallable)
	void SetAlarm(float DangerLevel);

	/** Fire */
	UFUNCTION(Category=Character, BlueprintCallable)
	void Fire();
	
	// Begin MechanicBot Interface
	bool TakeMicrowave(float Power) override;
	const class UPrimitiveComponent* GetShape() const override;
	void OnCorruptedStart();
	void OnCorruptedEnd();
	void OnKaput();
	// End MechanicBot Interface

public:

	/** Behaviour tree asset which controls this pat (containt the graph and blackboard asset) */
	UPROPERTY(Category = AI, EditDefaultsOnly, BlueprintReadOnly)
	class UBehaviorTree* Behavior;

	/** Level of danger when this pat enters the alarm state [0; 1] */
	UPROPERTY(Category = AI, EditDefaultsOnly, BlueprintReadOnly)
	float AlarmLevel;

	/** Max walk speed when patrolling */
	UPROPERTY(Category = AI, EditDefaultsOnly, BlueprintReadOnly)
	float PatrolLocomotionSpeed;

	/** Max walk speed when chasing */
	UPROPERTY(Category = AI, EditDefaultsOnly, BlueprintReadOnly)
	float AlarmLocomotionSpeed;

	/** Max pitch tilt angle when accelerating, degrees */
	UPROPERTY(Category = AI, EditDefaultsOnly, BlueprintReadOnly)
	float MaxPitchTilt;

	/** Max roll tilt angle when accelerating, degrees */
	UPROPERTY(Category = AI, EditDefaultsOnly, BlueprintReadOnly)
	float MaxRollTilt;

	/** How fast the body responds to the acceleration change */
	UPROPERTY(Category = AI, EditDefaultsOnly, BlueprintReadOnly)
	float TiltSpeed;

	/** Tag of the waypoints this pat wants to patrol along */
	UPROPERTY(Category = AI, EditAnywhere, BlueprintReadWrite)
	FString WaypointTag;

protected:
	
	/** Weapon */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class UMWBotWeaponComponent* Weapon;

	/** Meash socket name for the weapon */
	UPROPERTY(Category = Weapon, EditDefaultsOnly, BlueprintReadOnly)
	FName WeaponSocket;

	/** The volume which consumes MW radiation */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class UCapsuleComponent* Shape;

	/** Pat's "eye" and "ear" */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class UMWBotSensorComponent* Sensor;

	/** Mesh socket where the sensor attached to */
	UPROPERTY(Category = Sensor, EditDefaultsOnly, BlueprintReadOnly)
	FName SensorSocket;

	/** Pat's vision cone (attached to the eye) */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class UMWLightconeMeshComponent* Lightcone;

	/** Lightcone material */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UMaterialInterface* LightconeMaterial;

	/** Spot light component */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class USpotLightComponent* SpotLight;

	/** Static mesh which immitates jet fire shape (left and right) */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class UStaticMeshComponent* JetFireMesh[2];

	/** Socket names of the body mesh where jet fire meshes are attached to (left and right) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName JetFireSocket[2];

	/** Health component */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class UMWBotHealthComponent* HealthComponent;

private:
		
private:

	/** Initial body mesh rotation */
	UPROPERTY(Transient)
	FRotator DefaultBodyRotation;

	/** Actor velocity in the previous frame (needed for the accel. calculation) */
	UPROPERTY(Transient)
	FVector LastVelocity;

	/** Whether crazy */
	UPROPERTY(Transient)
	uint8 bCrazy : 1;

};
