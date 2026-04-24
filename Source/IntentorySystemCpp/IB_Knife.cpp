// Fill out your copyright notice in the Description page of Project Settings.

#include "IB_Knife.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstance.h"

AIB_Knife::AIB_Knife()
{
	ItemName = TEXT("Knife");
	ItemDescription = TEXT("Melee blade. Requires the Melee Adept title to take out of the bag.");
	RequiredTitleToUse = TEXT("\uADFC\uC811 \uB2EC\uC778");

	// 기본 크기 설정 (가로 1칸 x 세로 2칸)
	Dimensions = FIntPoint(1, 2);

	// 아이콘 머티리얼 경로 지정
	// TODO: 경로를 실제 프로젝트의 머티리얼 경로로 교체하세요
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> IconFinder(
		TEXT("/Game/InventoryResources/MaterialIcons/MI_Knife"));
	if (IconFinder.Succeeded())
	{
		Icon = IconFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> IconRotated(
		TEXT("/Game/InventoryResources/MaterialIcons/RotatedIcons/MI_Rotated_Knife"));
	if (IconRotated.Succeeded())
	{
		RotatedIcon = IconRotated.Object;
	}
}
