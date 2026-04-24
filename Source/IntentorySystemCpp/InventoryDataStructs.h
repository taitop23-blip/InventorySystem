// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryDataStructs.generated.h"

USTRUCT()
struct FLines 
{
	GENERATED_USTRUCT_BODY();

	FLines()
	{

	};

	TArray<FVector2D> XLines;
	TArray<FVector2D> YLines;
};

/** TArray 가방에 올라온 아이템을 조회할 때 TMap의 Value로 노출할 상세 정보 */
USTRUCT(BlueprintType)
struct FItemDetailInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString RequiredTitle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FIntPoint Dimensions = FIntPoint::ZeroValue;
};

class INTENTORYSYSTEMCPP_API InventoryDataStructs
{
public:

};
