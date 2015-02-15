// K.A. Aliev

#include "MicroWave.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

AMWPat::AMWPat(const FObjectInitializer& OI)
	: Super(OI)
{
	AIControllerClass = AMWPatController::StaticClass();

	// Want smooth rotation
	bUseControllerRotationYaw = false;

	// Pat
	AlarmLevel = 0.2f;
	PatrolLocomotionSpeed = 300;
	AlarmLocomotionSpeed = 600;
	MaxPitchTilt = 45.f;
	MaxRollTilt = 45.f;
	TiltSpeed = 2.f;

	// Movement component
	UCharacterMovementComponent* mover = GetCharacterMovement();
	mover->MaxWalkSpeed = PatrolLocomotionSpeed;
	mover->bUseControllerDesiredRotation = false;
	mover->bOrientRotationToMovement = true;
	mover->RotationRate = FRotator(0, 90, 0);

	// Step up component
	UCapsuleComponent* stepup = GetCapsuleComponent();
	stepup->InitCapsuleSize(95.0f, 95.0f);
	stepup->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	stepup->SetCollisionObjectType(ECollisionChannel::ECC_Vehicle);
	stepup->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	stepup->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

	// Mesh component
	USkeletalMeshComponent* mesh = GetMesh();
	mesh->AttachParent = stepup;
	mesh->SetRelativeLocation(FVector(0, 0, 200.f));
	mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	mesh->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	// MW sensible shape
	Shape = OI.CreateDefaultSubobject<UCapsuleComponent>(this, TEXT("Shape"));
	Shape->AttachParent = mesh;
	Shape->InitCapsuleSize(40.0f, 70.0f);
	Shape->SetRelativeLocation(FVector(-10.f, 0.f, 5.f));
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
	Sensor->AlarmReaction = 0.f;
	Sensor->WanderAngle = 3.f;
	Sensor->SeeDistance = 3000.f;
	Sensor->bEnableSensorWander = true;

	// Lightcone
	Lightcone = OI.CreateDefaultSubobject<UMWLightconeMeshComponent>(this, TEXT("Lightcone"));
	Lightcone->AttachParent = Sensor;

	// Spot light
	SpotLight = OI.CreateDefaultSubobject<USpotLightComponent>(this, TEXT("SpotLight"));
	SpotLight->AttachParent = Lightcone;

	// Weapon
	Weapon = OI.CreateDefaultSubobject<UMWBotWeaponComponent>(this, TEXT("Weapon"));
	Weapon->AttachParent = Sensor;

	// Jet fire meshes (attachment upon archetype init)
	JetFireMesh[0] = OI.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("JetFireMesh_Left"));
	JetFireMesh[1] = OI.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("JetFireMesh_Right"));
	JetFireMesh[0]->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	JetFireMesh[1]->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	// Health component
	HealthComponent = OI.CreateDefaultSubobject<UMWBotHealthComponent>(this, TEXT("HealthComponent"));
}

void AMWPat::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// attach whatever to the body mesh
	if (GetMesh())
	{
		Sensor->AttachTo(GetMesh(), SensorSocket, EAttachLocation::SnapToTarget, false);
	//	Weapon->AttachTo(GetMesh(), WeaponSocket, EAttachLocation::SnapToTarget, false);

		JetFireMesh[0]->AttachTo(GetMesh(), JetFireSocket[0], EAttachLocation::SnapToTarget, false);
		JetFireMesh[1]->AttachTo(GetMesh(), JetFireSocket[1], EAttachLocation::SnapToTarget, false);
	}

	GetCharacterMovement()->MaxWalkSpeed = PatrolLocomotionSpeed;

	DefaultBodyRotation = GetMesh()->GetRelativeTransform().Rotator();

	Lightcone->CreateAndSetMaterialInstanceDynamicFromMaterial(0, LightconeMaterial);
}

void AMWPat::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ApplyTilt(DeltaSeconds);
	Float();

	Lightcone->AdjustLight(SpotLight);
}

void AMWPat::LookAt(const AActor* Actor, const FVector Point)
{
	Sensor->LookAt(Actor, Point);
}

void AMWPat::ApplyTilt(float DeltaTime)
{

	// calc acceleration
	const FVector velocity = GetRootComponent()->GetComponentVelocity();
	const FVector velOrt = velocity.SafeNormal();
	const FVector accel = (velocity - LastVelocity) * (1.f / DeltaTime);
	const float maxAccel = GetCharacterMovement()->MaxAcceleration;

	// calc pitch and roll tilt
	const float pitchAng = - MaxPitchTilt * (velOrt | accel) / maxAccel;
	const float rollAng = MaxRollTilt * (velOrt ^ accel) | FVector(0.f, 0.f, 1.f) / maxAccel;

	// interpolate between current and calculated tilt
	const FRotator currentRot = GetMesh()->GetRelativeTransform().Rotator();
	const FRotator targetRot = FRotator(pitchAng, 0.f, rollAng) + DefaultBodyRotation;
	const FRotator newRot = FMath::RInterpTo(currentRot, targetRot, DeltaTime, TiltSpeed);

	GetMesh()->SetRelativeRotation(newRot);

	LastVelocity = velocity;
}

void AMWPat::Float()
{
	const static FVector defaultMeshLocation = GetMesh()->GetRelativeTransform().GetLocation();

	// calc body deviation
	const float time = GWorld->GetTimeSeconds();
	const float deviationExtent = 10.f; // cm
	const float horisDeviation = FMath::Cos(0.8f * PI * time) * deviationExtent;
	const float vertDeviation = FMath::Cos(0.4f * PI * time) * deviationExtent;
	
	const FVector deviation(0.f, horisDeviation, vertDeviation);
	const FVector newLoc = defaultMeshLocation + deviation;

	if (GetMesh()->GetAttachParent())
	{
		GetMesh()->SetRelativeLocation(newLoc);
	}
}

void AMWPat::Relax()
{
	SetAlarm(0.f);

	Sensor->LookStraight();
	Sensor->SetAlarm(false);

	GetCharacterMovement()->MaxWalkSpeed = PatrolLocomotionSpeed;
}

void AMWPat::SetAlarm(float DangerLevel)
{
	static const float defaultConeRadius = Lightcone->OuterRadius;

	if (bCrazy)
	{
		return;
	}

	if (DangerLevel >= AlarmLevel)
	{
		GetCharacterMovement()->MaxWalkSpeed = AlarmLocomotionSpeed;
		Sensor->SetAlarm(true);
	}

	// shrink lightcone outer radius when Level -> 1
	const float alpha = FMath::Clamp(DangerLevel, 0.f, 1.f);
	Lightcone->OuterRadius = FMath::Lerp(defaultConeRadius, Lightcone->InnerRadius + 0.1f, alpha);
}

void AMWPat::Fire()
{
	Weapon->Fire();
}

bool AMWPat::TakeMicrowave(float Power)
{
	return HealthComponent->TakeMicrowave(Power);
}

const UPrimitiveComponent* AMWPat::GetShape() const
{
	return Shape;
}

void AMWPat::OnCorruptedStart()
{
	// Forget the stranger, slow down locomotion ect.
	Relax();

	// Shake the sensor in a crazy way
	Sensor->GoCrazy(true);

	bCrazy = true;
}

void AMWPat::OnCorruptedEnd()
{
	Sensor->GoCrazy(false);

	bCrazy = false;
}

void AMWPat::OnKaput()
{
	USkeletalMeshComponent* mesh = GetMesh();

	// Turn on physics on the mesh to simulate a fall
	mesh->DetachFromParent(true);
	mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Ignore); // ignore the root capsule
	mesh->SetSimulatePhysics(true);

	// Add spinning effect
	mesh->SetPhysicsMaxAngularVelocity(50001.f);
	mesh->SetPhysicsAngularVelocity(FVector(-1000.f, 0.f, 1000.f));

	// Turn off the lightcone
	Lightcone->SetVisibility(false);

	// Turn off jets
	JetFireMesh[0]->SetVisibility(false);
	JetFireMesh[1]->SetVisibility(false);

	// Turn off light
	SpotLight->SetVisibility(false);

	// Disable ticking the actor
	SetActorTickEnabled(false);
	
	// Stop the behavior tree
	AMWPatController* Controller = Cast<AMWPatController>(GetController());
	if (Controller && Controller->GetBehaviorTree())
	{
		Controller->GetBehaviorTree()->StopTree();
	}
}


