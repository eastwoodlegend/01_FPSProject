// Will Pechell

#include "FPSProject.h"
#include "FPSHUD.h"
#include "Engine/Canvas.h"

void AFPSHUD::DrawHUD()
{
	Super::DrawHUD();

	if (CrosshairTexture)
	{
		FVector2D Center(Canvas->ClipX * .5f, Canvas->ClipY * .5f);

		FVector2D CrosshairDrawPosition(Center.X - (CrosshairTexture->GetSurfaceWidth() * .5f), Center.Y - (CrosshairTexture->GetSurfaceHeight() * .5f));

		FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTexture->Resource, FLinearColor::White);

		TileItem.BlendMode = SE_BLEND_Translucent;

		Canvas->DrawItem(TileItem);
	}

	auto AmmoText = FString("Ammo: ");
	AmmoText.AppendInt(Ammo);

	DrawText(AmmoText, FLinearColor::Yellow, 0.f, 0.f);
}

void AFPSHUD::UpdateAmmo(int Value)
{
	Ammo = Value;
}