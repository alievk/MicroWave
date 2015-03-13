// K. A. Aliev

#include "MicroWave.h"
#include "AI/MWBotHealthComponent.h"
#include "Interface/Interface_MechanicBot.h"

UMWBotHealthComponent::UMWBotHealthComponent(const FObjectInitializer& OI)
	: Super(OI)
{
	Health = 100.f;
	Vulnerability = 0.6f;
	DeadlyMWPower = 49.f;
	RepairRate = 0.f;
	FullRepairAfter = 0.01f;
	CorruptionHealthLevel = 99.9f;

	// Can tick
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	bIsActive = true;
}

void UMWBotHealthComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (Health < 100.f && IsAlive())
	{
		// Soft repair
		if (RepairRate > 0.f)
		{
			Health = FMath::Min(100.f, Health + RepairRate * DeltaTime);
		}
		
		// Hard repair
		if (bCorrupted && (LastCorrupted + FullRepairAfter) < GWorld->GetTimeSeconds())
		{
			bCorrupted = false;
			Health = 100.f;

			// Notify the owner it is repaired
			IInterface_MechanicBot* Owner = Cast<IInterface_MechanicBot>(GetOwner());
			if (Owner)
			{
				Owner->OnCorruptedEnd();
			}
		}
	}
}

bool UMWBotHealthComponent::TakeMicrowave(float Power)
{
	float DeltaTime = GWorld->GetDeltaSeconds();

	// Decrease health according to the vulnerability
	Health = FMath::Max(0.f, Health - Power * DeltaTime * Vulnerability);

	// Is corrupted?
	if (IsAlive())
	{
		if (CorruptionHealthLevel > 0.f && CorruptionHealthLevel >= Health)
		{
			bCorrupted = true;
			LastCorrupted = GWorld->GetTimeSeconds();

			// Notify the owner about the corruption
			IInterface_MechanicBot* Owner = Cast<IInterface_MechanicBot>(GetOwner());
			if (Owner)
			{
				Owner->OnCorruptedStart();
			}
		}

		// Die if received maximum MW radiation dose
		if (DeadlyMWPower > 0.f && Power > DeadlyMWPower)
		{
			Die();
		}
	}

	return IsAlive();
}

void UMWBotHealthComponent::Die()
{ 
	Health = 0.f;
	bCorrupted = true;

	// Notify the owner abouth the death
	IInterface_MechanicBot* Owner = Cast<IInterface_MechanicBot>(GetOwner());
	if (Owner)
	{
		Owner->OnKaput();
	}
}

bool UMWBotHealthComponent::IsAlive() const
{
	return Health > 0.f;
}

bool UMWBotHealthComponent::IsCorrupted() const
{ 
	return bCorrupted;
}

