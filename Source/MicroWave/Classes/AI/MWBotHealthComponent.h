// K. A. Aliev

#pragma once

#include "MWBotHealthComponent.generated.h"

/**
* Compenent containing vitality information for the electronic bots (pats, mots etc.)
*/
UCLASS()
class MICROWAVE_API UMWBotHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	/** Default UObject constructor */
	UMWBotHealthComponent(const FObjectInitializer& OI);

	// Begin UActorComponent Interface
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// End UActorComponent Interface

	/** Decreases health by [Power * DeltaTime * Vulnerability] and returns true if still alive.
	* @param Power - MW radiation power
	*/
	bool TakeMicrowave(float Power);

	/** Sets health = 0 */
	UFUNCTION(BlueprintCallable, Category = Health)
	void Die();

	/** Returns true if Health != 0 */
	UFUNCTION(BlueprintCallable, Category = Health)
	bool IsAlive() const;

	/** Returns true if Health is below the corruption level */
	UFUNCTION(BlueprintCallable, Category = Health)
	bool IsCorrupted() const;

protected:

	/** Health points [0; 100] */
	UPROPERTY(BlueprintReadOnly, Category=Health)
	float Health;

	//** Repair rate (health points in sec) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Health)
	float RepairRate;

	/** Vulnerability to MW [0; 1]: 0 - not vulnerable at all, 1 - takes all the radiation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Health)
	float Vulnerability;

	/** Deadly amout of radiation it can take at once (if <= 0, then no limit) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Health)
	float DeadlyMWPower;

	/** Health level when the owner bot should corrupt [0; 100] */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Health)
	float CorruptionHealthLevel;

	/** Full repair time after corruption, sec */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Health)
	float FullRepairAfter;

private:

	/** Is felt out */
	UPROPERTY(Transient)
	uint8 bCorrupted : 1;

	/** When felt out */
	UPROPERTY(Transient)
	float LastCorrupted;
};
