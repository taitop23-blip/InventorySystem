// Fill out your copyright notice in the Description page of Project Settings.
// InventoryComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryDataStructs.h"
#include "InventoryComponent.generated.h"

class AItemBase;
class UInventoryGridWidget;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class INTENTORYSYSTEMCPP_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category = "IC Info | Inventory Columns")
	int32 Columns;

	UPROPERTY(EditAnywhere, Category = "IC Info | Inventory Rows")
	int32 Rows;

	UPROPERTY(EditAnywhere, Category = "IC Info | Inventory TileSize")
	float TileSize;

	bool AddedItem = false;

	UInventoryGridWidget* InventoryGridWidgetReference;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IC Info | Bag")
	TArray<AItemBase*> Items;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IC Info | Item Info Map")
	TMap<FString, FItemDetailInfo> ItemInfoByName;

	bool TryAddItem(AItemBase* ItemToAdd);

	bool IsRoomAvailable(AItemBase* ItemToAdd, int32 Index);

	bool IsRoomAvailableForPayload(AItemBase* ItemToAdd, int32 TopLeftIndex, AItemBase* ItemToExclude);

	FIntPoint IndexToTile(int32 TopLeftIndex);

	bool IsTileValid(FIntPoint Tile);

	int32 TileToIndex(FIntPoint Tile);

	bool GetResultAtIndex(int32 Index);

	AItemBase* GetItemAtIndex(int32 Index);

	void AddItemAt(AItemBase* ItemToAdd, int32 TopLeftIndex);

	TMap<AItemBase*, FIntPoint> GetAllItems();

	void SetInventoryGridWidget(UInventoryGridWidget* GridWidgetReference);

	void RemoveItem(AItemBase* ItemToRemove);

	void RefreshAllItems();

protected:
	TMap<AItemBase*, FIntPoint> AllItems;

	void RebuildItemInfoByNameFromGrid();

	virtual void BeginPlay() override;
};
