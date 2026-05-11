#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "IntentorySystemCppHUD.generated.h"

UCLASS()
class INTENTORYSYSTEMCPP_API AIntentorySystemCppHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void DrawHUD() override;
};
