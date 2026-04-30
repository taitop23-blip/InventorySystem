// Fill out your copyright notice in the Description page of Project Settings.

#include "IB_AK47.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstance.h"

AIB_AK47::AIB_AK47()
{
	ItemName = TEXT("AK-47");
	ItemDescription = TEXT("Automatic rifle. Requires the Weapons Expert title to take out of the bag.");
	RequiredTitleToUse = TEXT("\uBB34\uAE30 \uC804\uBB38\uAC00");

	Damage = 18.f;
	Range = 12000.f;
	FireRate = 0.12f;
	PelletCount = 1;
	SpreadAngle = 1.2f;
	RecoilPitch = 0.7f;
	RecoilYaw = 0.18f;
	RecoilRecoverySpeed = 10.f;
	TraceChannel = ECC_Pawn;

	// �⺻ ũ�� ���� (���� 2ĭ x ���� 4ĭ)
	Dimensions = FIntPoint(2, 4);

	// ������ ��Ƽ���� ��� ����
	// TODO: ��θ� ���� ������Ʈ�� ��Ƽ���� ��η� ��ü�ϼ���
	// ��: TEXT("/Game/Icons/M_AK47_Icon")
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
