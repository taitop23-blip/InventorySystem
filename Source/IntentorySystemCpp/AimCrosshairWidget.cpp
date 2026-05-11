#include "AimCrosshairWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"

int32 UAimCrosshairWidget::NativePaint(const FPaintArgs& Args,
	const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled) const
{
	LayerId = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	FPaintContext PaintContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	const FVector2D Size = AllottedGeometry.GetLocalSize();
	const FVector2D C = Size * 0.5f;
	const float Arm = 18.f;
	const float Gap = 5.f;
	const FLinearColor Color(1.f, 0.92f, 0.2f, 0.92f);
	const float Thickness = 2.2f;

	UWidgetBlueprintLibrary::DrawLine(PaintContext, C + FVector2D(0.f, -Arm), C + FVector2D(0.f, -Gap), Color, true, Thickness);
	UWidgetBlueprintLibrary::DrawLine(PaintContext, C + FVector2D(0.f, Gap), C + FVector2D(0.f, Arm), Color, true, Thickness);
	UWidgetBlueprintLibrary::DrawLine(PaintContext, C + FVector2D(-Arm, 0.f), C + FVector2D(-Gap, 0.f), Color, true, Thickness);
	UWidgetBlueprintLibrary::DrawLine(PaintContext, C + FVector2D(Gap, 0.f), C + FVector2D(Arm, 0.f), Color, true, Thickness);

	return LayerId;
}
