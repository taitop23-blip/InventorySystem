// Fill out your copyright notice in the Description page of Project Settings.

#include "IB_Grenade.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstance.h"

AIB_Grenade::AIB_Grenade()
{
	ItemName = TEXT("Grenade");
	ItemDescription = TEXT("Throwable explosive. Requires the Explosives Permit title.");
	RequiredTitleToUse = TEXT("\uD3ED\uBC1C\uBB3C \uBA74\uD5C8");

	// �⺻ ũ�� ���� (���� 1ĭ x ���� 1ĭ)
	Dimensions = FIntPoint(1, 1);

	// ������ ��Ƽ���� ��� ����
	// TODO: ��θ� ���� ������Ʈ�� ��Ƽ���� ��η� ��ü�ϼ���
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> IconFinder(
		TEXT("/Game/InventoryResources/MaterialIcons/MI_Grenade"));
	if (IconFinder.Succeeded())
	{
		Icon = IconFinder.Object;
	}
}
