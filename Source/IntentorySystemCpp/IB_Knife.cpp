// Fill out your copyright notice in the Description page of Project Settings.

#include "IB_Knife.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstance.h"

AIB_Knife::AIB_Knife()
{
	ItemName = TEXT("Knife");
	ItemDescription = TEXT("Melee blade. Requires the Melee Adept title to take out of the bag.");
	RequiredTitleToUse = TEXT("\uADFC\uC811 \uB2EC\uC778");

	// �⺻ ũ�� ���� (���� 1ĭ x ���� 2ĭ)
	Dimensions = FIntPoint(1, 2);

	// ������ ��Ƽ���� ��� ����
	// TODO: ��θ� ���� ������Ʈ�� ��Ƽ���� ��η� ��ü�ϼ���
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
