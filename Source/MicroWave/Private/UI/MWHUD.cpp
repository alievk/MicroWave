// K.A. Aliev

#include "MicroWave.h"

AMWHUD::AMWHUD(const FObjectInitializer& OI) : Super(OI)
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> HUDMainTextureOb(TEXT("/Game/UI/HUD/HUDMain"));

	static ConstructorHelpers::FObjectFinder<UFont> BigFontOb(TEXT("/Game/UI/HUD/Roboto51"));
	static ConstructorHelpers::FObjectFinder<UFont> NormalFontOb(TEXT("/Game/UI/HUD/Roboto18"));

	HUDMainTexture = HUDMainTextureOb.Object;
	BigFont = BigFontOb.Object;
	NormalFont = NormalFontOb.Object;

	Crosshair = UCanvas::MakeIcon(HUDMainTexture, 20, 20, 64, 64);
}


void AMWHUD::DrawHUD()
{
	Super::DrawHUD();

	AMWCharacter* MyPawn = CastChecked<AMWCharacter>(GetOwningPawn());
	AMWGun* MyGun = MyPawn->GetGun();

	ScaleUI = Canvas->ClipY / 1080.f;

	float CenterX = Canvas->ClipX / 2;
	float CenterY = Canvas->ClipY / 2;
	
	// Draw crosshair
	Canvas->SetDrawColor(255, 255, 255, 192);
	Canvas->DrawIcon(Crosshair,
		CenterX - Crosshair.UL*ScaleUI / 2.0f,
		CenterY - Crosshair.VL*ScaleUI / 2.0f, ScaleUI);

	// Draw gun charge

	//int32 gunCharge = int(100.f * MyGun->GetBattery()->GetCurrentCharge() / MyGun->GetBattery()->Capacity);
	FString Text = FString::FromInt(MyPawn->GetHealth()); // FString::FromInt(gunCharge);

	float SizeX, SizeY;
	FCanvasTextItem TextItem(FVector2D::ZeroVector, FText::GetEmpty(), BigFont, FLinearColor::White);
	//TextItem.EnableShadow(FLinearColor::Black);
	Canvas->StrLen(BigFont, Text, SizeX, SizeY);

	float TextScale = 0.73f;	// of 51 font
	float TextPosX = Canvas->ClipX - Canvas->OrgX - SizeX;
	float TextPosY = Canvas->ClipY - Canvas->OrgY - SizeY;
	TextItem.Text = FText::FromString(Text);
	TextItem.Scale = FVector2D(TextScale * ScaleUI, TextScale * ScaleUI);
	//TextItem.FontRenderInfo = ShadowedFont;
	Canvas->DrawItem(TextItem, TextPosX, TextPosY);
}

