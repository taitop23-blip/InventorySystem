// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"
#include "IntentorySystemCpp.h"
#include "ItemBase.h"
#include "InventoryGridWidget.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	Items.Init(nullptr, Columns * Rows);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("InventoryComponent Ready"));
	UE_LOG(LogIntentoryAssignment, Log, TEXT("InventoryComponent: TArray bag initialized (%d x %d = %d cells)."),
		Columns, Rows, Items.Num());
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (AddedItem)
	{
		if (InventoryGridWidgetReference)
		{
			InventoryGridWidgetReference->Refresh();
		}
		AddedItem = false;
	}
}

bool UInventoryComponent::TryAddItem(AItemBase* ItemToAdd)
{
	if (!ItemToAdd) return false;

	// ?????????? ???? ???
	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (IsRoomAvailable(ItemToAdd, i))
		{
			AddItemAt(ItemToAdd, i);
			return true;
		}
	}

	// [???7] ???? ?????? ??? ?? ????
	ItemToAdd->Rotate();
	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (IsRoomAvailable(ItemToAdd, i))
		{
			AddItemAt(ItemToAdd, i);
			return true;
		}
	}

	// ?? ?? ??????? ???? ???????? ?????
	ItemToAdd->Rotate();
	return false;
}

bool UInventoryComponent::IsRoomAvailable(AItemBase* ItemToAdd, int32 TopLeftIndex)
{
	FIntPoint Dimensions = ItemToAdd->GetDimensions();
	FIntPoint Tile = IndexToTile(TopLeftIndex);

	for (int32 i = Tile.X; i <= Tile.X + Dimensions.X - 1; i++)
	{
		for (int32 j = Tile.Y; j <= Tile.Y + Dimensions.Y - 1; j++)
		{
			if (IsTileValid(FIntPoint(i, j)))
			{
				int32 Index = TileToIndex(FIntPoint(i, j));
				if (GetResultAtIndex(Index))
				{
					if (GetItemAtIndex(Index))
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
	}
	return true;
}

// [???7] ????? ??? ?? ??? ???(???????)?? ??????? ???? ???
bool UInventoryComponent::IsRoomAvailableForPayload(AItemBase* ItemToAdd, int32 TopLeftIndex, AItemBase* ItemToExclude)
{
	FIntPoint Dimensions = ItemToAdd->GetDimensions();
	FIntPoint Tile = IndexToTile(TopLeftIndex);

	for (int32 i = Tile.X; i <= Tile.X + Dimensions.X - 1; i++)
	{
		for (int32 j = Tile.Y; j <= Tile.Y + Dimensions.Y - 1; j++)
		{
			if (IsTileValid(FIntPoint(i, j)))
			{
				int32 Index = TileToIndex(FIntPoint(i, j));
				if (GetResultAtIndex(Index))
				{
					AItemBase* ItemAtIndex = GetItemAtIndex(Index);
					// ??? ????? ????
					if (ItemAtIndex && ItemAtIndex != ItemToExclude)
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
	}
	return true;
}

FIntPoint UInventoryComponent::IndexToTile(int32 Index)
{
	return FIntPoint(Index % Columns, Index / Columns);
}

bool UInventoryComponent::IsTileValid(FIntPoint Tile)
{
	return (Tile.X >= 0 && Tile.Y >= 0 && Tile.X < Columns && Tile.Y < Rows);
}

int32 UInventoryComponent::TileToIndex(FIntPoint Tile)
{
	return Tile.X + Tile.Y * Columns;
}

bool UInventoryComponent::GetResultAtIndex(int32 Index)
{
	return Items.IsValidIndex(Index);
}

AItemBase* UInventoryComponent::GetItemAtIndex(int32 Index)
{
	if (Items.IsValidIndex(Index))
	{
		return Items[Index];
	}
	return nullptr;
}

void UInventoryComponent::AddItemAt(AItemBase* ItemToAdd, int32 TopLeftIndex)
{
	FIntPoint Dimensions = ItemToAdd->GetDimensions();
	FIntPoint Tile = IndexToTile(TopLeftIndex);

	for (int32 i = Tile.X; i <= Tile.X + Dimensions.X - 1; i++)
	{
		for (int32 j = Tile.Y; j <= Tile.Y + Dimensions.Y - 1; j++)
		{
			Items[TileToIndex(FIntPoint(i, j))] = ItemToAdd;
		}
	}
	AddedItem = true;
}

void UInventoryComponent::RebuildItemInfoByNameFromGrid()
{
	ItemInfoByName.Empty();
	TSet<AItemBase*> Seen;
	for (AItemBase* Cell : Items)
	{
		if (!Cell || Seen.Contains(Cell))
		{
			continue;
		}
		Seen.Add(Cell);

		const FString Key = Cell->ItemName;
		if (Key.IsEmpty() || ItemInfoByName.Contains(Key))
		{
			continue;
		}

		FItemDetailInfo Info;
		Info.DisplayName = Cell->ItemName;
		Info.Description = Cell->ItemDescription;
		Info.RequiredTitle = Cell->RequiredTitleToUse;
		Info.Dimensions = Cell->GetDimensions();
		ItemInfoByName.Add(Key, Info);
	}
}

TMap<AItemBase*, FIntPoint> UInventoryComponent::GetAllItems()
{
	AllItems.Empty();
	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (Items[i])
		{
			if (!AllItems.Contains(Items[i]))
			{
				AllItems.Add(Items[i], IndexToTile(i));
			}
		}
	}
	RebuildItemInfoByNameFromGrid();
	return AllItems;
}

void UInventoryComponent::SetInventoryGridWidget(UInventoryGridWidget* GridWidgetReference)
{
	InventoryGridWidgetReference = GridWidgetReference;
}

void UInventoryComponent::RemoveItem(AItemBase* ItemToRemove)
{
	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (Items[i] == ItemToRemove)
		{
			Items[i] = nullptr;
		}
	}
}

void UInventoryComponent::RefreshAllItems()
{
	AllItems.Empty();
}
