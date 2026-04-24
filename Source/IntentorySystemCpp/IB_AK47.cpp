// Fill out your copyright notice in the Description page of Project Settings.

#include "IB_AK47.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstance.h"

AIB_AK47::AIB_AK47()
{
	ItemName = TEXT("AK-47");
	ItemDescription = TEXT("Automatic rifle. Requires the Weapons Expert title to take out of the bag.");
	RequiredTitleToUse = TEXT("\uBB34\uAE30 \uC804\uBB38\uAC00");

	// 기본 크기 설정 (가로 2칸 x 세로 4칸)
	Dimensions = FIntPoint(2, 4);

	// 아이콘 머티리얼 경로 지정
	// TODO: 경로를 실제 프로젝트의 머티리얼 경로로 교체하세요
	// 예: TEXT("/Game/Icons/M_AK47_Icon")
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> IconFinder(
		TEXT("/Game/InventoryResources/MaterialIcons/MI_AK47"));
	if (IconFinder.Succeeded())
	{
		Icon = IconFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> IconRotated(
		TEXT("/Game/InventoryResources/MaterialIcons/RotatedIcons/MI_Rotated_AK47"));
	if (IconRotated.Succeeded())
	{
		RotatedIcon = IconRotated.Object;
	}
}
