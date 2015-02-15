// K.A. Aliev

#pragma once
#include "MWBotWeaponComponent.generated.h"

/**
 * This component represents bot's weapon basic logic.
 */
UCLASS()
class MICROWAVE_API UMWBotWeaponComponent: public USceneComponent
{
	GENERATED_BODY()

public:

	/** Default UObject constructor. */
	UMWBotWeaponComponent(const FObjectInitializer& OI);

	// Begin UObject Interface
	//void PostLoad() override;
	// End UObject Interface

	// Begin UActorComponent Interface
	//void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// End UActorComponent Interface

	/**
	* Cast a line forward.
	* @return Hit result.
	*/
	UFUNCTION(Category = BotWeapon, BlueprintCallable)
	void Fire();

public:

	/** Period between shots, seconds */
	UPROPERTY(Category = BotWeapon, EditDefaultsOnly, BlueprintReadOnly)
	float FirePeriod;

	/** Period between shot bursts (0 means no burst), second */
	UPROPERTY(Category = BotWeapon, EditDefaultsOnly, BlueprintReadOnly)
	float BurstPeriod;

	/** Number of shots in a bursts */
	UPROPERTY(Category = BotWeapon, EditDefaultsOnly, BlueprintReadOnly)
	int32 BurstCount;

	/** Weapon spread, degrees */
	UPROPERTY(Category = BotWeapon, EditDefaultsOnly, BlueprintReadOnly)
	float Spread;

	/** Weapon strength, [0, 1] */
	UPROPERTY(Category = BotWeapon, EditDefaultsOnly, BlueprintReadOnly)
	float Strength;

private:

	/** Returns true if the weapon can fire */
	bool CanFire();

	/** Check whether the player was hit and hurt him */
	void ProcessHit(FHitResult Hit);
		
private:

	/** Last shot time */
	UPROPERTY(Transient)
	float LastShot;

	/** Last shot time */
	UPROPERTY(Transient)
	float LastBurst;

	/** How much has shot during a burst */
	UPROPERTY(Transient)
	int32 ShotNumber;
};
