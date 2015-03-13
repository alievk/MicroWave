// K. A. Aliev

#pragma once

#include "MWGun.generated.h"

/**
 * The microwave gun class.
 */
UCLASS(Config = Game)
class MICROWAVE_API AMWGun : public AActor
{
	GENERATED_BODY()

public:

	/** Default UObject constructor */
	AMWGun(const FObjectInitializer& OI);

	/** Do blueprint-initialization dependent setup */
	void PostInitializeComponents() override;

	// Begin AActor Interface
	void Tick(float DeltaTime) override;
	// End AActor Interface

	/** Start fire */
	void StartFire();

	/** Do actual shooting 
	* @param bAlternative - If true, alternative fire
	*/
	void FireGun(float DeltaTime);

	/** Start alternative fire */
	void StartAltFire();

	/** Stop fire */
	void StopFire();

	/** Set owning pawn */
	void SetOwningPawn(class AMWCharacter* NewOwner);

	/** Called upon equipment */
	void OnEquip();

	/** Returns battery of this gun */
	UFUNCTION(BlueprintCallable, Category=Gun)
	class AMWBattery* GetBattery() const;

	/** Get gun aim direction */
	FVector GetShootingDirection() const;

	/** Get gun trace start point */
	FVector GetMuzzleLocation() const;

private:

	/** Returns true if gun can shoot */
	bool CanShoot() const;

public:

	/** MWBattery class used by this gun */
	UPROPERTY(Category = Gun, EditDefaultsOnly)
	TSubclassOf<class AMWBattery> MWBatteryClass;

	/** How far gun hits (in UE units) */
	UPROPERTY(EditDefaultsOnly, Category = Gun)
	float GunRange;

	/** Name for bone/socket of the mesh used as a muzzle point */
	UPROPERTY(EditDefaultsOnly, Category = Gun)
	FName MuzzleSocketName;

	/** Name for bone/socket of the mesh for plugging a cable */
	UPROPERTY(EditDefaultsOnly, Category = Gun)
	FName CableSocketName;

	/** If true, alternative fire is enabled */
	UPROPERTY(EditDefaultsOnly, Category = Gun)
	bool EnableAltFire;

protected:

	/** Spawns battery of class MWBatteryClass */
	void SpawnBattery();

	/** Returns true if gun hits given Component */
	bool GunHitsComponent(const UPrimitiveComponent* Component) const;

protected:

	/** The character owning this gun */
	UPROPERTY(Transient)
	class AMWCharacter* MyPawn;

	/** Gun's mesh */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* GunMesh;

	/** RF-cone drawn when MWgun fires */
	UPROPERTY(VisibleDefaultsOnly, Category=Effects)
	class UMWLightconeMeshComponent* MWCone;

	/** MWCone material */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UMaterialInterface* MWConeMaterial;

	/** Current charge */
	UPROPERTY(BlueprintReadOnly)
	float CurrentCharge;

	/** True if shooting in normal mode */
	UPROPERTY(Transient)
	uint8 bShooting : 1;

	/** True if shooting in normal mode */
	UPROPERTY(Transient)
	uint8 bAltShooting : 1;

	/** This is timer which counts down when alt shooting, it lets to see that gun actually shoots for a short period */
	UPROPERTY(Transient)
	float AltShootingTimer;

private:

	/** MWCone material */
	UPROPERTY(Transient)
	class UMaterialInstanceDynamic* MWConeMID;

	/** Battery this gun uses */
	UPROPERTY(Transient)
	class AMWBattery* MyBattery;
};

// ////////////////////////////////////////////////////////////////////////////////////
// Backup

/** Check what gun hits when fires */
//FHitResult GunTrace(const FVector& StartTrace, const FVector& EndTrace) const;

/** Process what gut hit */
//void ProcessHit(const FHitResult& HitResult) const;

/** Energy capacity */
//UPROPERTY(EditDefaultsOnly, Category = Gun)
//float Capacity;

/** How fast energy is spent: energy' = energy - Power*dt */
//UPROPERTY(EditDefaultsOnly, Category = Gun)
//float Power;

/** Multiplier of Power upon Alternative Fire */
//UPROPERTY(EditDefaultsOnly, Category = Gun)
//float PowerMultiplier;

/** Returns true if can fire right now */
//bool CanFire() const;

/** Returns current charge in gun */
//float GetCurrentCharge() const;

/** How fast gun recharges: energy' = energy + RechargeRate*dt */
//UPROPERTY(EditDefaultsOnly, Category = Gun)
//float RechargeRate;