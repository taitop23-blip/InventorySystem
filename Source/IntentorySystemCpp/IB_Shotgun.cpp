#include "IB_Shotgun.h"

#include "Engine/Texture2D.h"
#include "UObject/ConstructorHelpers.h"

AIB_Shotgun::AIB_Shotgun()
{
	ItemName = TEXT("Shotgun");
	ItemDescription = TEXT("Close-range shotgun with multiple pellets and heavy recoil.");
	RequiredTitleToUse = TEXT("\uBB34\uAE30 \uC804\uBB38\uAC00");

	Dimensions = FIntPoint(2, 3);

	Damage = 9.f;
	Range = 5000.f;
	FireRate = 0.85f;
	PelletCount = 9;
	SpreadAngle = 7.5f;
	RecoilPitch = 2.8f;
	RecoilYaw = 0.75f;
	RecoilRecoverySpeed = 5.f;
	TraceChannel = ECC_Pawn;

	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTextureFinder(
		TEXT("/Game/InventoryResources/Shotgun/Shotgun"));
	if (IconTextureFinder.Succeeded())
	{
		IconTexture = IconTextureFinder.Object;
	}
}
