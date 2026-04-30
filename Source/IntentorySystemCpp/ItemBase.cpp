// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemBase.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

AItemBase::AItemBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	SetRootComponent(Sphere);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Sphere);

	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	Sphere->SetGenerateOverlapEvents(true);
}

void AItemBase::BeginPlay()
{
	Super::BeginPlay();
}

void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FIntPoint AItemBase::GetDimensions() const
{
	return Dimensions;
}

UMaterialInstance* AItemBase::GetIcon()
{
	// [파트7] 회전 상태에 따라 아이콘 반환
	if (IsRotated && RotatedIcon)
	{
		return RotatedIcon;
	}
	return Icon;
}

UTexture2D* AItemBase::GetIconTexture()
{
	if (IsRotated && RotatedIconTexture)
	{
		return RotatedIconTexture;
	}
	return IconTexture;
}

// [파트7] 아이템 회전: Dimensions X/Y 교환 + 상태 토글
void AItemBase::Rotate()
{
	IsRotated = !IsRotated;
	Dimensions = FIntPoint(Dimensions.Y, Dimensions.X);
}
