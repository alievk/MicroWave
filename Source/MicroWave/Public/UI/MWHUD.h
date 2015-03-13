// K.A. Aliev

#pragma once 

#include "MWHUD.generated.h"

UCLASS()
class AMWHUD : public AHUD
{
	GENERATED_UCLASS_BODY()

public:

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:

	/** Main HUD texture */
	UPROPERTY(Transient)
	class UTexture2D* HUDMainTexture;

	/** Crosshair */
	UPROPERTY(Transient)
	FCanvasIcon Crosshair;

	/** Large font */
	UPROPERTY()
	UFont* BigFont;

	/** Normal font */
	UPROPERTY()
	UFont* NormalFont;

	/** UI scaling factor */
	float ScaleUI;
};

