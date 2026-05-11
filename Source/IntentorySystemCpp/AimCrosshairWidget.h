// Aim crosshair drawn at widget center (viewport-aligned in code).

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AimCrosshairWidget.generated.h"

UCLASS()
class INTENTORYSYSTEMCPP_API UAimCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual int32 NativePaint(const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;
};
