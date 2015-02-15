// K.A. Aliev

#pragma once
#include "MWBotSensorComponent.generated.h"

/**
 * This component represents bot's eye and ear.
 */
UCLASS()
class MICROWAVE_API UMWBotSensorComponent: public USceneComponent
{
	GENERATED_BODY()

public:

	/** Default UObject constructor. */
	UMWBotSensorComponent(const FObjectInitializer& OI);

	// Begin UObject Interface
	void PostLoad() override;
	// End UObject Interface

	// Begin UActorComponent Interface
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// End UActorComponent Interface

	/** Returns true if there is a line of sight between this component and an Actor in the acceptable distance (see SeeDistance).
	* @param OutCanHear - Is true only if there is a LOS and the Actor is close enough (see HearDistance) */
	UFUNCTION(Category=BotSensor, BlueprintCallable)
	bool CheckLOS(const AActor* Actor, bool& OutCanHear) const;

	/** Returns true if there this component can "see" an Actor in a given angle (degrees) */
	UFUNCTION(Category=BotSensor, BlueprintCallable)
	bool CheckAOS(const AActor* Actor, float Angle) const;

	/** Try to target sensor to the given actor or location, if actor is null. */
	UFUNCTION(Category=BotSensor, BlueprintCallable)
	void LookAt(const AActor* Actor, const FVector Point);

	/** Make the sensor to look straight (reset rotation) */
	UFUNCTION(Category=BotSensor, BlueprintCallable)
	void LookStraight();

	/**
	* Changes reaction speed, wandering parameters etc.
	* @param NewState - If true switch to alarm state, else to normal.
	*/
	UFUNCTION(Category = BotSensor, BlueprintCallable)
	void SetAlarm(bool NewState);

	/** Returns true if aralm */
	UFUNCTION(BlueprintCallable, Category = BotSensor)
	bool IsAlarm() const;
	
	/**
	* Shakes in a crazy way
	* @param NewState - If true, go crazy.
	*/
	UFUNCTION(Category = BotSensor, BlueprintCallable)
	void GoCrazy(bool NewState);

public:

	/** This angle constraints LookAt() operation (degrees) */
	UPROPERTY(Category = BotSensor, EditDefaultsOnly, BlueprintReadOnly)
	float ViewingAngle;

	/** How fast sensor changes its orientation in normal state. If 0, reaction is maximal.*/
	UPROPERTY(Category = BotSensor, EditDefaultsOnly)
	float Reaction;

	/** How fast sensor changes its orientation in alarm state. If 0, reaction is maximal. */
	UPROPERTY(Category = BotSensor, EditDefaultsOnly)
	float AlarmReaction;

	/** How fast sensor changes its orientation when crazy. If 0, reaction is maximal. */
	UPROPERTY(Category = BotSensor, EditDefaultsOnly)
	float CrazyReaction;

	/** If true, enables sensor random orientation change (parameters are based on the alarm state). */
	UPROPERTY(Category = AI, EditDefaultsOnly, BlueprintReadOnly)
	uint8 bEnableSensorWander : 1;

	/** How often sensor changes its random orientation, when wandering in normal state, seconds */
	UPROPERTY(Category = BotSensor, EditDefaultsOnly)
	float WanderPeriod;

	/** How often sensor changes its random orientation, when wandering in alarm state, seconds */
	UPROPERTY(Category = BotSensor, EditDefaultsOnly)
	float AlarmWanderPeriod;

	/** How often sensor changes its random orientation, when crazy, seconds */
	UPROPERTY(Category = BotSensor, EditDefaultsOnly)
	float CrazyWanderPeriod;

	/** Max wander angle (equals to ViewingAngle by default), degrees. */
	UPROPERTY(Category = BotSensor, EditDefaultsOnly)
	float WanderAngle;

	/** Max wander angle in alarm state, degrees.  */
	UPROPERTY(Category = BotSensor, EditDefaultsOnly)
	float AlarmWanderAngle;

	/** Max wander angle when crazy, degrees.  */
	UPROPERTY(Category = BotSensor, EditDefaultsOnly)
	float CrazyWanderAngle;

	/** How far pat can see */
	UPROPERTY(Category = BotSensor, EditDefaultsOnly, BlueprintReadOnly)
	float SeeDistance;

	/** How far pat can hear */
	UPROPERTY(Category = BotSensor, EditDefaultsOnly, BlueprintReadOnly)
	float HearDistance;

private:

	/** Smoothly update the rotation and clamp the angle if necessary*/
	void UpdateRotation(float DeltaSeconds);

	/** Applies random angle to the given rotator
	* @param Rotator - rotator to apply random angle to. 
	* @return New rotator with a random distortion. */
	FRotator ApplyRandomAngle(FRotator Rotator);
		
private:

	/** Default orientation (relative) */
	FRotator DefaultRotation;

	/** Desired orientation (relative) */
	FRotator DesiredRotation;

	/** Last time sensor wandered */
	UPROPERTY(Transient)
	float LastWander;

	/** Last random angle applied to the orientation */
	UPROPERTY(Transient)
	FRotator LastWanderRotator;

	/** If true, sensor uses Alarm* parameters */
	UPROPERTY(Transient)
	uint8 bAlarm : 1;

	/** If true, sensor ignores LookAt(), CheckLOS() and so on. */
	UPROPERTY(Transient)
	uint8 bCrazy : 1;
};
