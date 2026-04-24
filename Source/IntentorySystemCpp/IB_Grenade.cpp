// Fill out your copyright notice in the Description page of Project Settings.

#include "IB_Grenade.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstance.h"

AIB_Grenade::AIB_Grenade()
{
	ItemName = TEXT("Grenade");
	ItemDescription = TEXT("Throwable explosive. Requires the Explosives Permit title.");
	RequiredTitleToUse = TEXT("\uD3ED\uBC1C\uBB3C \uBA74\uD5C8");

	// 기본 크기 설정 (가로 1칸 x 세로 1칸)
	Dimensions = FIntPoint(1, 1);

	// 아이콘 머티리얼 경로 지정
	// TODO: 경로를 실제 프로젝트의 머티리얼 경로로 교체하세요
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> IconFinder(
		TEXT("/Game/InventoryResources/MaterialIcons/MI_Grenade"));
	if (IconFinder.Succeeded())
	{
		Icon = IconFinder.Object;
	}
}
