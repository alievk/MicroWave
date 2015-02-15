// Fill out your copyright notice in the Description page of Project Settings.

#include "MicroWave.h"

UMWBotSensorComponent::UMWBotSensorComponent(const FObjectInitializer& OI)
	: Super(OI)
{
	// Can tick
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	bIsActive = true;

	// Some default values
	SeeDistance = 2000.f;
	HearDistance = 1000.f;
	ViewingAngle = WanderAngle = 30.f;
	Reaction = 1.f;
	AlarmReaction = 0.f;
	CrazyReaction = 10.f;
	WanderPeriod = 1.f;
	AlarmWanderPeriod = 0.1f;
	CrazyWanderPeriod = 0.1f;
	WanderAngle = 10.f;
	AlarmWanderAngle = 1.f;
	CrazyWanderAngle = 10.f;
}

void UMWBotSensorComponent::PostLoad()
{
	Super::PostLoad();

	// Remember the default (straight) rotation. We need it the sensor is not attached to a socket.
	DefaultRotation = DesiredRotation = GetRelativeTransform().Rotator();
}

void UMWBotSensorComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateRotation(DeltaTime);
}

bool UMWBotSensorComponent::CheckLOS(const AActor* Actor, bool& OutCanHear) const
{
	if (!Actor/* || bCrazy*/)
	{
		return false;
	}

	bool canSee = false;

	float dist = (GetComponentLocation() - Actor->GetActorLocation()).Size();
	if (dist < SeeDistance)
	{
		FHitResult hit;
		FCollisionQueryParams params;
		params.AddIgnoredActor(GetOwner());

		GetWorld()->LineTraceSingle(hit, GetComponentLocation(), Actor->GetActorLocation(), ECollisionChannel::ECC_Visibility, params);
	
		canSee = hit.bBlockingHit && (Actor == hit.Actor);
	}

	OutCanHear = canSee && (dist < HearDistance);

	return canSee;
}

bool UMWBotSensorComponent::CheckAOS(const AActor* Actor, float Angle) const
{
	// Calculate dot product of two vector and check the angle between them

	if (!Actor/* || bCrazy*/)
	{
		return false;
	}

	const FVector toActor = (Actor->GetActorLocation() - GetComponentLocation()).SafeNormal();
	const FVector sightOrt = GetComponentRotation().Vector();
	const float dotProd = toActor | sightOrt;
	const float cosA = FMath::Cos(FMath::DegreesToRadians(Angle));

	return dotProd > cosA;
}

void UMWBotSensorComponent::LookAt(const AActor* Actor, const FVector Point)
{
	//if (bCrazy)
	//{
	//	return;
	//}

	// Convert aim direction to eye parent's local space 

	FVector aimDirWS = Actor ? Actor->GetActorLocation() : Point; // world space
	aimDirWS -= GetComponentLocation();
	FVector aimDirLS; // local space

	if (AttachParent)
	{
		// If attached to socket, use socket transform
		if (!AttachSocketName.IsNone())
		{
			aimDirLS = AttachParent->GetSocketTransform(AttachSocketName, ERelativeTransformSpace::RTS_World).InverseTransformVectorNoScale(aimDirWS);
		}
		else
		{
			aimDirLS = AttachParent->GetComponentTransform().InverseTransformVectorNoScale(aimDirWS);
		}
	}
	else
	{
		aimDirLS = aimDirWS;
	}
		
	DesiredRotation = aimDirLS.Rotation();
}

void UMWBotSensorComponent::LookStraight()
{
	// if snaped to a socket, just reset the rotation
	if (!AttachSocketName.IsNone())
	{
		DesiredRotation = FRotator::ZeroRotator;
	}
	else
	{
		// restore original angle offset
		DesiredRotation = DefaultRotation;
	}
}

void UMWBotSensorComponent::UpdateRotation(float DeltaSeconds)
{
	// desRot is DesiredRotation with optionally added random angle
	const FRotator curRot = GetRelativeTransform().Rotator();
	FRotator desRot = bEnableSensorWander ? ApplyRandomAngle(DesiredRotation) : DesiredRotation;

	// clamp the aiming angle (desired rotation)

	const FVector defAim = DefaultRotation.Vector();
	const FVector desAim = desRot.Vector();

	const float maxAngRad = PI * ViewingAngle / 180.f;
	const float curCos = defAim | desAim;
	const float minCos = FMath::Cos(maxAngRad);
	if (curCos < minCos) // out of range
	{
		// Find quaternion which rotates the eye in the aimDef-aimDir plane
		const FVector axis = defAim ^ desAim;
		const FQuat quat(axis, maxAngRad);

		desRot = quat.RotateVector(defAim).Rotation();
	}

	// smoothly change the sensor aiming
	const float interpSpeed = bCrazy ? CrazyReaction : bAlarm ? AlarmReaction : Reaction;
	FRotator newRot = FMath::RInterpTo(curRot, desRot, DeltaSeconds, interpSpeed);
	SetRelativeRotation(newRot);
}

FRotator UMWBotSensorComponent::ApplyRandomAngle(FRotator Rotator)
{
	const float period = bCrazy ? CrazyWanderPeriod : bAlarm ? AlarmWanderPeriod : WanderPeriod;
	const float time = GWorld->GetTimeSeconds();

	if (time > (LastWander + period) && period > 0)
	{
		const int32 randSeed = FMath::Rand();
		FRandomStream randStream(randSeed);
		const float range = bCrazy ? CrazyWanderAngle : bAlarm ? AlarmWanderAngle : WanderAngle;

		const FVector randV = randStream.VRandCone(FVector(1, 0, 0), FMath::DegreesToRadians(range));
		const FRotator randR = randV.Rotation();
		
		LastWander = time;
		LastWanderRotator = randR;

		return Rotator + randR;
	}

	return Rotator + LastWanderRotator;
}

void UMWBotSensorComponent::SetAlarm(bool NewState)
{
	bAlarm = NewState;
}

bool UMWBotSensorComponent::IsAlarm() const
{
	return bAlarm;
}

void UMWBotSensorComponent::GoCrazy(bool NewState)
{
	bCrazy = NewState;
}

