// K. A. Aliev

#include "MicroWave.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

//static UMaterialInterface* MotLightconeMaterial;

AMWMot::AMWMot(const FObjectInitializer& OI)
	: Super(OI)
{
//	AIControllerClass = AMWMotController::StaticClass();

	GuardReaction = 5.f;
	AlarmReaction = 0.f; // no delay
	GuardAngleLower = FRotator(-35.f, -60.f, 0.f);
	GuardAngleUpper = FRotator(-10.f, 60.f, 0.f);
	GuardPointChangePeriod = 4.f;
	AlarmLevel = 0.2f;
	AimRotation = FRotator::ZeroRotator;

	// Mesh component
	Mesh = OI.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Mesh"));
	Mesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	//Mesh->AttachParent = Base;
	Mesh->bCastDynamicShadow = true;
	Mesh->bAffectDynamicIndirectLighting = true;
	Mesh->bChartDistanceFactor = true;
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Ignore);

	Mesh->bGenerateOverlapEvents = false;
	Mesh->bCanEverAffectNavigation = false;

	RootComponent = Mesh;

	// Weapon (attached after blueprint init)
	Weapon = OI.CreateDefaultSubobject<UMWBotWeaponComponent>(this, TEXT("Weapon"));
	Weapon->BurstCount = 4;
	Weapon->Strength = 25.f;

	// MW sensible shape
	Shape = OI.CreateDefaultSubobject<UCapsuleComponent>(this, TEXT("Shape"));
	Shape->AttachParent = Mesh;
	Shape->InitCapsuleSize(28.f, 44.f);
	Shape->SetRelativeLocation(FVector(50.f, 0.f, -20.f));
	Shape->bGenerateOverlapEvents = false;
	Shape->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	Shape->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Shape->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
	Shape->bGenerateOverlapEvents = false;
	Shape->bCanEverAffectNavigation = false;

	// Sensor (attached after blueprint init)
	Sensor = OI.CreateDefaultSubobject<UMWBotSensorComponent>(this, TEXT("Sensor"));
	Sensor->ViewingAngle = 60.f;
	Sensor->Reaction = 10.f;
	Sensor->AlarmReaction = 50.f;
	Sensor->WanderAngle = 3.f;
	Sensor->WanderPeriod = 0.5f;
	Sensor->CrazyWanderPeriod = 0.05f;
	Sensor->CrazyWanderAngle = 40.f;
	Sensor->SeeDistance = 5000.f;
	Sensor->bEnableSensorWander = true;

	// Lightcone
	Lightcone = OI.CreateDefaultSubobject<UMWLightconeMeshComponent>(this, TEXT("Lightcone"));
	Lightcone->AttachParent = Sensor;
	Lightcone->InnerRadius = 2.f;
	Lightcone->OuterRadius = 400.f;
	Lightcone->Distance = 3000.f;
	Lightcone->MeshUSegments = 40;
	Lightcone->MeshVSegments = 24;

	// Spot light
	SpotLight = OI.CreateDefaultSubobject<USpotLightComponent>(this, TEXT("SpotLight"));
	SpotLight->AttachParent = Lightcone;

	// Health component
	HealthComponent = OI.CreateDefaultSubobject<UMWBotHealthComponent>(this, TEXT("HealthComponent"));
}

void AMWMot::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// attach the sensor and the weapon to the mesh
	Sensor->AttachTo(Mesh, SensorSocket, EAttachLocation::SnapToTarget, false);
	Weapon->AttachTo(Mesh, WeaponSocket, EAttachLocation::SnapToTarget, false);

	// Allow wander from the beginning
	LastGuardPointChange = -GuardPointChangePeriod;

	Lightcone->CreateAndSetMaterialInstanceDynamicFromMaterial(0, LightconeMaterial);
}

void AMWMot::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateRotation(DeltaSeconds);

	Lightcone->AdjustLight(SpotLight);
}

bool AMWMot::TakeMicrowave(float Power)
{
	return HealthComponent->TakeMicrowave(Power);
}

const UPrimitiveComponent* AMWMot::GetShape() const
{
	return Shape;
}

void AMWMot::OnCorruptedStart()
{
	Relax();

	// Shake the sensor in a crazy way
	Sensor->GoCrazy(true);

	bCrazy = true;
}

void AMWMot::OnCorruptedEnd()
{
	Sensor->GoCrazy(false);

	bCrazy = false;
}

void AMWMot::OnKaput()
{
	// Enable physics for the mesh
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Ignore); // Vehicle = root capsule component of Pat
	Mesh->SetSimulatePhysics(true);

	// Turn off the lightcone
	Lightcone->SetVisibility(false, true);

	// Disable ticking the actor
	SetActorTickEnabled(false);

	// Stop the behavior tree
	AAIController* controller = Cast<AAIController>(GetController());
	if (controller)
	{
		UBehaviorTreeComponent* behavior = Cast<UBehaviorTreeComponent>(controller->BrainComponent);
		if (behavior)
		{
			behavior->StopTree();
		}
	}
}

bool AMWMot::LookAt(const AActor* Target, const FVector Point, const float Tolerance)
{
	// Aim the sensor at the target | point.
	Sensor->LookAt(Target, Point);

	const FVector SensorLoc = Sensor->GetComponentLocation();
	FVector AimWS = Target ? Target->GetActorLocation() - SensorLoc : Point - SensorLoc; // world-space aim vector

	// Translate Aim to mot's local coordinate space and get its rotator
	const FVector AimLS = GetTransform().InverseTransformVectorNoScale(AimWS); // local-space aim vector
	const FRotator AimDir = AimLS.Rotation();

	// Clamp the angles
	const float PitchLimit = 120.f;
	const float YawLimit = 120.f;
	DesiredAimRotation.Pitch = FMath::ClampAngle(AimDir.Pitch, -PitchLimit, PitchLimit);
	DesiredAimRotation.Yaw = FMath::ClampAngle(AimDir.Yaw, -YawLimit, YawLimit);
	DesiredAimRotation.Roll = 0.f;

	// Check whether aimed in a given tolerance
	bool equalPitch = FMath::IsNearlyEqual(DesiredAimRotation.Pitch, AimRotation.Pitch, Tolerance);
	bool equalYaw = FMath::IsNearlyEqual(DesiredAimRotation.Yaw, AimRotation.Yaw, Tolerance);
	return equalPitch && equalYaw;
}

void AMWMot::Guard()
{
	const float time = GWorld->GetTimeSeconds();

	if (time > LastGuardPointChange + GuardPointChangePeriod)
	{
		// pick random angle and set as desired

		const int32 randSeed = FMath::Rand();
		FRandomStream randStream(randSeed);

		const float newPitch = randStream.FRandRange(GuardAngleLower.Pitch, GuardAngleUpper.Pitch);
		const float newYaw = randStream.FRandRange(GuardAngleLower.Yaw, GuardAngleUpper.Yaw);
		DesiredAimRotation = FRotator(newPitch, newYaw, 0.f);

		LastGuardPointChange = time;
	}
}

void AMWMot::SetAlarm(float DangerLevel)
{
	static const float defaultConeRadius = Lightcone->OuterRadius;

	if (bCrazy)
	{
		return;
	}

	if (DangerLevel >= AlarmLevel)
	{
		Sensor->SetAlarm(true);
	}
	else
	{
		Sensor->SetAlarm(false);
	}

	// shrink lightcone outer radius when Level -> 1
	const float alpha = FMath::Clamp(DangerLevel, 0.f, 1.f);
	Lightcone->OuterRadius = FMath::Lerp(defaultConeRadius, Lightcone->InnerRadius + 0.1f, alpha);
}

void AMWMot::Relax()
{
	Sensor->LookStraight();

	SetAlarm(0.f);
}

void AMWMot::Fire()
{
	Weapon->Fire();
}

void AMWMot::UpdateRotation(float DeltaSeconds)
{
	// smoothly change Aim Rotation (for Anim Blueprint)
	const float interpSpeed = Sensor->IsAlarm() ? AlarmReaction : GuardReaction;
	AimRotation = FMath::RInterpTo(AimRotation, DesiredAimRotation, DeltaSeconds, interpSpeed);
}


// ///////////////////////////////////////////////////////
// Backup

// Base
//Base = OI.CreateDefaultSubobject<USceneComponent>(this, TEXT("Base"));
//RootComponent = Base;

// MeshConstraint (initialization in PostInitializeComponents)
//MeshConstraint = OI.CreateDefaultSubobject<UPhysicsConstraintComponent>(this, TEXT("MeshConstraint"));
//MeshConstraint->ComponentName1.ComponentName = TEXT("Base");
//MeshConstraint->ComponentName2.ComponentName = TEXT("Mesh");
//MeshConstraint->ConstraintInstance.ConstraintBone1 = NAME_None;
//MeshConstraint->ConstraintInstance.ConstraintBone2 = TEXT("root");
//MeshConstraint->ConstraintInstance.bDisableCollision = true;
//MeshConstraint->ConstraintInstance.AngularSwing1Motion = EAngularConstraintMotion::ACM_Locked;
//MeshConstraint->ConstraintInstance.AngularSwing2Motion = EAngularConstraintMotion::ACM_Locked;
//MeshConstraint->ConstraintInstance.AngularTwistMotion = EAngularConstraintMotion::ACM_Locked;

// Tear off the mot's body from its mount point
// [ mot ]
//   ||
//   ||-( o ) --> tear off force
//
// TODO: get rid of hardcoded values
//const float tearStrength = 1500.f;
//const FVector AimWS = GetTransform().TransformVectorNoScale(AimRotation.Vector());
//const FVector tearDir =  AimWS ^ FVector(0.f, 0.f, -1.f); // vector pointing rightwards from the aiming direction
//const FVector tearImpulse = tearDir * tearStrength;
//FName boneToBreak = TEXT("pitch");
//Mesh->AddImpulse(tearImpulse, boneToBreak, true);

// Add spinning effect
//Mesh->SetPhysicsMaxAngularVelocity(50001.f);
//Mesh->SetPhysicsAngularVelocity(FVector(-1000.f, 0.f, 1000.f));