// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBase.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UMaterialInstance;
class UTexture2D;

UCLASS()
class INTENTORYSYSTEMCPP_API AItemBase : public AActor
{
	GENERATED_BODY()

public:
	AItemBase();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	USphereComponent* Sphere;

	FIntPoint GetDimensions() const;

	UMaterialInstance* GetIcon();

	UTexture2D* GetIconTexture();

	// [��Ʈ7] ȸ�� ���
	bool IsRotated = false;
	void Rotate();

	UPROPERTY(EditAnywhere, Category = "Item Info | Name")
	FString ItemName = TEXT("Item");

	UPROPERTY(EditAnywhere, Category = "Item Info | Description", meta = (MultiLine = "true"))
	FString ItemDescription;

	/** ?? ??? ?? ?? ?? ??? ?? ??(??) ?? */
	UPROPERTY(EditAnywhere, Category = "Item Info | Title")
	FString RequiredTitleToUse;

	UPROPERTY(EditAnywhere, Category = "Item Info | Dimensions")
	FIntPoint Dimensions;

	UPROPERTY(EditAnywhere, Category = "Item Info | Icon")
	UMaterialInstance* Icon;

	// [��Ʈ7] ȸ�� �� ����� ������
	UPROPERTY(EditAnywhere, Category = "Item Info | Icon")
	UMaterialInstance* RotatedIcon;

	UPROPERTY(EditAnywhere, Category = "Item Info | Icon")
	UTexture2D* IconTexture;

	UPROPERTY(EditAnywhere, Category = "Item Info | Icon")
	UTexture2D* RotatedIconTexture;

protected:
	virtual void BeginPlay() override;
};
