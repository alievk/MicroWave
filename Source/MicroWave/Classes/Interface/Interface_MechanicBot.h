// K.A. Aliev

#pragma once
#include "Interface_MechanicBot.generated.h"

/** Interface for mechanic bots */
UINTERFACE(MinimalApi, meta = (CannotImplementInterfaceInBlueprint))
class UInterface_MechanicBot : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IInterface_MechanicBot
{
	GENERATED_IINTERFACE_BODY()

	/** Take microwave damage */
	virtual bool TakeMicrowave(float Power) = 0;

	/** Returns the collision shape of the bot which takes mw radiation (usually capsule) */
	virtual const class UPrimitiveComponent* GetShape() const = 0;

	/** Called upon the bot being temporarily disabled (corrupted) */
	virtual void OnCorruptedStart() = 0;

	/** Called upon the bot exiting disabled state */
	virtual void OnCorruptedEnd() = 0;

	/** Called upon the bot being completely disabled (kaput) */
	virtual void OnKaput() = 0;
};

