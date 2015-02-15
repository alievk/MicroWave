// K. A. Aliev

#pragma once

#include "MWCharacter.generated.h"
 
UCLASS(config=Game)
class AMWCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	/** Default UObject constructor */
	AMWCharacter(const FObjectInitializer& OI);

	/** Spawn inventory, setup initial variables */
	void PostInitializeComponents() override;

	// Begin AActor Interface
	void Tick(float DeltaTime) override;
	// End AActor Interface
	
	// ///////////////////////////////////////////////////////////////
	// Inventory

	/** Spawn & equip a gun of a new class */
	void AddGun(TSubclassOf<class AMWGun> GunClass);

	/** Equip a gun */
	void EquipGun(class AMWGun* Gun);

	/** Returns currently equipped gun */
	class AMWGun* GetGun() const;

	/** Returns name of socket on characters skeleton where gun is attached to */
	FName GetGunSocketName() const;

	/** Returns name of socket on character's skeleton where battery is attached to */
	FName GetBatterySocketName() const;

	// ///////////////////////////////////////////////////////////////
	// Control

	// Begin APawn Interface
	void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End APawn Interface

	/** Handles StartFire action */
	void OnStartFire();

	/** Handles StartAltFire action */
	void OnStartAltFire();

	/** Handles StopFire action */
	void OnStopFire();

	/** Move character forward/backward */
	void MoveForward(float Scale);

	/** Move character right/left */
	void MoveRight(float Scale);

	// ///////////////////////////////////////////////////////////////
	// Health

	/** Hurt this character to decrease its health 
	* @param Pain - how much health points to decrease
	* @return True, if still alive after being hurt
	*/
	bool Hurt(int32 Pain);

	/** Returns true if this character is alive */
	UFUNCTION(BlueprintCallable, Category = Default)
	bool IsAlive() const;

	/** Returns health level */
	float GetHealth() const { return Health;  }
	
	// ///////////////////////////////////////////////////////////////
	// Auxilary

	/** Returns true if the character produces noise at the time */
	UFUNCTION(BlueprintCallable, Category=Default)
	bool IsNoisy() const;

protected:

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=Character)
	class USpringArmComponent* CameraBoom;
	
	/** Camera */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=Character)
	class UCameraComponent* CameraComponent;

	// ///////////////////////////////////////////////////////////////
	// Inventory

	/** Default MWGun equipped when the game just started */
	UPROPERTY(Category=Gun, EditDefaultsOnly)
	TSubclassOf<class AMWGun> DefaultMWGun;

	UPROPERTY(Transient)
	TArray<class AMWGun*> Inventory;

	/** Currently equipped tool */
	UPROPERTY(BlueprintReadOnly, Category=Gun)
	class AMWGun* CurrentGun;

	/** Socket or bone name in body mesh for attaching gun mesh */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	FName GunSocketName;

	/** Socket or bone name in body mesh for attaching battery */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	FName BatterySocketName;



	// ///////////////////////////////////////////////////////////////
	// Control

	/** If true, turns body if the angle between the aim and the actor is greater than the aiming yaw angle limit */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Camera)
	uint8 bTurnBodyToAim : 1;

	// ///////////////////////////////////////////////////////////////
	// Health

	/** Health: 0-100 */
	int32 Health;

private:

	/** Turn the body towards the aim if the hands aiming angle limit is reached */
	void TurnToAim();

private:

};

// /////////////////////////////////////////////////////
// Backup

/** Point in the world the character should aim */
	//UPROPERTY(BlueprintReadOnly, Category = Control)
	//FVector Aim;
	
	// ///////////////////////////////////////////////////////////////
	// Effects

	///** Sound to play each time we fire */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	//USoundBase* FireSound;

	///** AnimMontage to play each time we fire */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	//UAnimMontage* FireAnimation;

/** Turn character (root component)
	* @param AddAngle - angle applied to the yaw rotation, degrees.
	*/
	//void Turn(float AddAngle);

	/** Set the point in the world the character should aim (set by MWPlayerController) */
	//void SetAim(FVector InAim) { Aim = InAim; }

	/** Set the point in the world the character should aim (set by MWPlayerController) */
	//UFUNCTION(BlueprintCallable, Category = Default)
	//FVector GetAim() const;

	/** Battery mesh */
	/*UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=Character)
	class UStaticMeshComponent* BatteryMesh;*/

	/** Cable connecting the battery and the gun */
	//UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=Character)
	//class UCableComponent* CableComponent;

	/** Component to which the cable attachs its beginning. Meanwhile, this component attached to the battery. */
	//UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=Character)
	//class UStaticMeshComponent* CableRoot;

	///** Component to which the 1st cable attachs its ending and the 2nd attachs its beginnign. Meanwhile, this component attached to the body mesh socket (for collision immitation). */
	//UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=Character)
	//class UStaticMeshComponent* CableEnd1;

	///** Component to which the 2nd cable attachs its ending. Meanwhile, this component attached to the gun's cable socket. */
	//UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=Character)
	//class UStaticMeshComponent* CableEnd2;

/** Socket or bone name in body mesh where cable touch */
//UPROPERTY(EditDefaultsOnly, Category = Inventory)
//FName CableTouchSocketName;