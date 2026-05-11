#include "IntentorySystemCppHUD.h"

#include "IntentorySystemCppCharacter.h"
#include "GameFramework/PlayerController.h"

void AIntentorySystemCppHUD::DrawHUD()
{
	Super::DrawHUD();

	APlayerController* PC = GetOwningPlayerController();
	if (!PC || !Canvas)
	{
		return;
	}

	AIntentorySystemCppCharacter* Char = Cast<AIntentorySystemCppCharacter>(PC->GetPawn());
	if (!Char || !Char->IsAiming())
	{
		return;
	}

	int32 VX = 0;
	int32 VY = 0;
	PC->GetViewportSize(VX, VY);
	const float CX = VX * 0.5f;
	const float CY = VY * 0.5f;
	const float Arm = 24.f;
	const float Gap = 7.f;

	const FLinearColor Outline(0.f, 0.f, 0.f, 0.92f);
	const FLinearColor Main(1.f, 0.93f, 0.12f, 1.f);

	auto DrawCross = [this, CX, CY, Arm, Gap](const FLinearColor& Color, float Thickness)
	{
		DrawLine(CX, CY - Arm, CX, CY - Gap, Color, Thickness);
		DrawLine(CX, CY + Gap, CX, CY + Arm, Color, Thickness);
		DrawLine(CX - Arm, CY, CX - Gap, CY, Color, Thickness);
		DrawLine(CX + Gap, CY, CX + Arm, CY, Color, Thickness);
	};

	DrawCross(Outline, 5.f);
	DrawCross(Main, 2.5f);
}
