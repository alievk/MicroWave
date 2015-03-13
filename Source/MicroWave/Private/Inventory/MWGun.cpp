// K. A. Aliev

#include "MicroWave.h"
#include "Inventory/MWGun.h"
#include "Inventory/MWBattery.h"
#include "Visual/MWLightconeMeshComponent.h"
#include "Interface/Interface_MechanicBot.h"
#include "Player/MWPlayerController.h"
#include "Player/MWCharacter.h"

AMWGun::AMWGun(const FObjectInitializer& OI)
	: Super(OI)
{
	// Mesh
	GunMesh = OI.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("GunMesh"));
	GunMesh->bChartDistanceFactor = false;
	GunMesh->bReceivesDecals = false;
	GunMesh->CastShadow = false;
	GunMesh->SetCollisionObjectType(ECC_WorldDynamic);
	GunMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GunMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootComponent = GunMesh;

	// RF-cone
	MWCone = OI.CreateDefaultSubobject<UMWLightconeMeshComponent>(this, TEXT("MWCone"));
	MWCone->MeshUSegments = 10;
	MWCone->MeshVSegments = 16;
	MWCone->bHiddenInGame = true;

	EnableAltFire = false;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
}

void AMWGun::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// MWCone (snaps to the muzzle socket)
	MWCone->Distance = GunRange;
	MWCone->AttachTo(GunMesh, MuzzleSocketName);

	MWCone->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MWConeMaterial);

	SpawnBattery();
}

void AMWGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (GEngine) GEngine->AddOnScreenDebugMessage(0, 1, FColor::Red, TEXT("Tick"));

	if (bShooting)
	{
		FireGun(DeltaTime);
	}

	if (AltShootingTimer > 0.f)
	{
		UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(MWCone->GetMaterial(0));
		if (MID)
		{
			// the higher the power and the AltShootingTimer value, the brighter waves color.
			const float mwPower = AltShootingTimer * MyBattery->GetCurrentPower() / MyBattery->MaxPower;
			MID->SetScalarParameterValue(TEXT("Power"), mwPower);
		}

		const float interpSpeed = 10.f;
		AltShootingTimer = FMath::FInterpTo(AltShootingTimer, 0.f, DeltaTime, interpSpeed);

		const float tolerance = 0.01;
		if (AltShootingTimer < tolerance)
		{
			MWCone->bHiddenInGame = true;

			AltShootingTimer = 0.f;
		}
	}
}

void AMWGun::StartFire()
{
	if (!CanShoot())
	{
		return;
	}

	bShooting = true;
	bAltShooting = false;

	UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(MWCone->GetMaterial(0));
	if (MID)
	{
		MID->SetScalarParameterValue(TEXT("Power"), 0.f); // 0 = normal power
	}
}

void AMWGun::FireGun(float DeltaTime)
{
	if (!CanShoot())
	{
		MWCone->bHiddenInGame = true;
		
		return;
	}

	// Orient the cone towards the shooting direction
	const FRotator coneRot = GetShootingDirection().Rotation();
	MWCone->SetWorldRotation(coneRot);

	// Get all bots in the world
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UInterface_MechanicBot::StaticClass(), actors);

	const float gunPower = MyBattery->ConsumeCharge(DeltaTime);

	// Check which bot(s) where hit by the mw radiation
	for (AActor* actor : actors)
	{
		IInterface_MechanicBot* bot = Cast<IInterface_MechanicBot>(actor);
		if (bot && GunHitsComponent(bot->GetShape()))
		{
			bot->TakeMicrowave(gunPower);
		}
	}

	// Show waves coming out of the gun
	MWCone->bHiddenInGame = false;	
}

void AMWGun::StartAltFire()
{
	if (CanShoot() && EnableAltFire)
	{
		bShooting = false;
		bAltShooting = true;

		MyBattery->OnPowerUpStart();
	}
}

void AMWGun::StopFire()
{
	if (bShooting)
	{
		// Hide waves coming out of the gun.
		MWCone->bHiddenInGame = true;

		bShooting = false;
	}

	// When alt shooting, shoot upon button release.
	if (bAltShooting)
	{
		AltShootingTimer = 1.f; // 1.f corresponds to "powerfull" color of MWCone, that's why I chose this value.

		MyBattery->OnPowerUpEnd();

		FireGun(1.f); // shoot only once and consume battery power immediatelly (for that DeltaTime = 1.f)
	
		bAltShooting = false;
	}
}

void AMWGun::SetOwningPawn(class AMWCharacter* NewOwner)
{
	if (MyPawn != NewOwner)
	{
		Instigator = NewOwner;
		MyPawn = NewOwner;
	}
}

void AMWGun::OnEquip()
{
	if (!MyPawn)
	{
		return;
	}

	// Attach gun mesh to the gun socket
	FName AttachPoint = MyPawn->GetGunSocketName();
	USkeletalMeshComponent* PawnMesh = MyPawn->GetMesh();
	GunMesh->AttachTo(PawnMesh, AttachPoint, EAttachLocation::SnapToTarget);
}

AMWBattery* AMWGun::GetBattery() const
{
	return MyBattery;
}

FVector AMWGun::GetShootingDirection() const
{
	//C    [GUN] o-- 
	//A               --  shooting direction
	//M                    --   __     |
	//E o----------------------------x | hit point
	//R        trace line              |
	//A

	AMWPlayerController* PC = MyPawn ? Cast<AMWPlayerController>(MyPawn->GetController()) : NULL;
	FVector ShootDir = FVector::ZeroVector;

	if (PC)
	{
		// Find camera trace line
		FVector StartTrace;
		FRotator CameraRot;
		PC->GetPlayerViewPoint(StartTrace, CameraRot);
		FVector EndTrace = StartTrace + CameraRot.Vector() * GunRange;

		// Trace a line from the camera and find hit point
		static const FName TraceTag = FName(TEXT("CameraTrace"));
		const FCollisionQueryParams TraceParams(TraceTag, false, Instigator);
		FHitResult Hit(ForceInit);
		GetWorld()->LineTraceSingle(Hit, StartTrace, EndTrace, ECollisionChannel::ECC_Visibility, TraceParams);
	
		// Check what was hit
		if (Hit.bBlockingHit)
		{
			ShootDir = (Hit.ImpactPoint - GetMuzzleLocation()).GetSafeNormal();
		}
		else
		{
			ShootDir = (EndTrace - StartTrace).GetSafeNormal();
		}
	}

	return ShootDir;
}

FVector AMWGun::GetMuzzleLocation() const
{
	return GunMesh->GetSocketLocation(MuzzleSocketName);
}

bool AMWGun::CanShoot() const
{
	return MyBattery && MyBattery->HasCharge();
}

void AMWGun::SpawnBattery()
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.bNoCollisionFail = true;
	MyBattery = GetWorld()->SpawnActor<AMWBattery>(MWBatteryClass, SpawnInfo);
}

bool AMWGun::GunHitsComponent(const UPrimitiveComponent* Component) const
{
	// Check whether MW-cone surface hits the Component
	if (MWCone->LightconeHitsComponent(Component))
	{
		return true;
	}

	// Before line trace, check whether Component is in the lightcone angle and near enough
	const float distance = (Component->GetComponentLocation() - GetMuzzleLocation()).Size();
	if (distance < GunRange && MWCone->PointInViewangle(Component->GetComponentLocation()))
	{
		// Trace a line from the muzzle to the component
		
		static const FName TraceTag = FName(TEXT("GunTrace"));
		const FCollisionQueryParams TraceParams(TraceTag, false, Instigator);
		FHitResult Hit;
		GetWorld()->LineTraceSingle(Hit, GetMuzzleLocation(), Component->GetComponentLocation(), ECollisionChannel::ECC_Camera, TraceParams);
	
		if (Hit.bBlockingHit && Hit.Component == Component)
		{
			return true;
		}
	}

	return false;
}

// ////////////////////////////////////////////////////////////////////////////////////
// Backup

//FHitResult AMWGun::GunTrace(const FVector& StartTrace, const FVector& EndTrace) const
//{
//	static const FName GunTraceTag = FName(TEXT("GunTrace"));
//
//	// Perform trace to retrieve hit info
//	const FCollisionQueryParams TraceParams(GunTraceTag, false, Instigator);
//
//	FHitResult Hit(ForceInit);
//	GetWorld()->LineTraceSingle(Hit, StartTrace, EndTrace, ECollisionChannel::ECC_Visibility, TraceParams);
//
//	// temporary trace draw
//	/*FVector StartLine = StartTrace;
//	FVector EndLine = Hit.bBlockingHit ? Hit.ImpactPoint : EndTrace;
//	DrawDebugLine(GetWorld(), StartLine, EndLine, FColor::Red, false, -1.f, 0, 5.f);*/
//
//	return Hit;
//}

//void AMWGun::ProcessHit(const FHitResult& HitResult) const
//{
//
//}

//bool AMWGun::CanFire() const
//{
//	return CurrentCharge > 0.f;
//}

//float AMWGun::GetCurrentCharge() const
//{
//	return CurrentCharge;
//}