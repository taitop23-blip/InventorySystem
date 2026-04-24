// Fill out your copyright notice in the Description page of Project Settings.
// InventoryGridWidget.cpp

#include "InventoryGridWidget.h"
#include "IntentorySystemCppCharacter.h"
#include "InventoryComponent.h"
#include "ItemBase.h"
#include "ItemWidget.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Styling/SlateBrush.h"

void UInventoryGridWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CharacterReference = Cast<AIntentorySystemCppCharacter>(GetOwningPlayerPawn());

	if (!CharacterReference || !CharacterReference->InventoryComponent)
		return;

	InventoryComponent = CharacterReference->InventoryComponent;

	Columns = InventoryComponent->Columns;
	Rows = InventoryComponent->Rows;
	TileSize = InventoryComponent->TileSize;

	float NewWidth = Columns * TileSize;
	float NewHeight = Rows * TileSize;

	UCanvasPanelSlot* BorderAsCanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(GridBorder);
	BorderAsCanvasSlot->SetSize(FVector2D(NewWidth, NewHeight));

	CreateLineSegments();

	InventoryComponent->SetInventoryGridWidget(this);
}

void UInventoryGridWidget::CreateLineSegments()
{
	for (int32 i = 0; i <= Columns; i++)
	{
		float X = i * TileSize;
		VertStartX.Add(X);  VertStartY.Add(0.0f);
		VertEndX.Add(X);    VertEndY.Add(Rows * TileSize);
	}
	for (int32 i = 0; i <= Rows; i++)
	{
		float Y = i * TileSize;
		HorzStartX.Add(0.0f);             HorzStartY.Add(Y);
		HorzEndX.Add(Columns * TileSize); HorzEndY.Add(Y);
	}
}

int32 UInventoryGridWidget::NativePaint(
	const FPaintArgs& Args,
	const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled) const
{
	Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	FPaintContext PaintContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	FLinearColor GridLineColor(0.5f, 0.5f, 0.5f, 0.5f);
	FVector2D TopLeftCorner = GridBorder->GetCachedGeometry().GetLocalPositionAtCoordinates(FVector2D(0.0f, 0.0f));

	for (int32 i = 0; i < VertStartX.Num(); i++)
	{
		FVector2D Start = FVector2D(VertStartX[i], VertStartY[i]) + TopLeftCorner;
		FVector2D End = FVector2D(VertEndX[i], VertEndY[i]) + TopLeftCorner;
		UWidgetBlueprintLibrary::DrawLine(PaintContext, Start, End, GridLineColor, false, 2.0f);
	}
	for (int32 i = 0; i < HorzStartX.Num(); i++)
	{
		FVector2D Start = FVector2D(HorzStartX[i], HorzStartY[i]) + TopLeftCorner;
		FVector2D End = FVector2D(HorzEndX[i], HorzEndY[i]) + TopLeftCorner;
		UWidgetBlueprintLibrary::DrawLine(PaintContext, Start, End, GridLineColor, false, 2.0f);
	}

	if (DraggingItem && CurrentDragTile.X >= 0 && CurrentDragTile.Y >= 0)
	{
		FIntPoint Dims = DraggingItem->GetDimensions();

		float BoxX = CurrentDragTile.X * TileSize + TopLeftCorner.X;
		float BoxY = CurrentDragTile.Y * TileSize + TopLeftCorner.Y;
		float BoxW = Dims.X * TileSize;
		float BoxH = Dims.Y * TileSize;

		FLinearColor BoxColor = bCanDrop
			? FLinearColor(0.0f, 1.0f, 0.0f, 0.3f)
			: FLinearColor(1.0f, 0.0f, 0.0f, 0.3f);

		FSlateBrush Brush;
		Brush.TintColor = FSlateColor(BoxColor);
		Brush.DrawAs = ESlateBrushDrawType::Image;

		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId + 1,
			AllottedGeometry.ToPaintGeometry(
				FVector2D(BoxW, BoxH),
				FSlateLayoutTransform(FVector2D(BoxX, BoxY))
			),
			&Brush,
			ESlateDrawEffect::None,
			BoxColor
		);
	}

	return LayerId;
}

bool UInventoryGridWidget::NativeOnDragOver(
	const FGeometry& InGeometry,
	const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	if (!InOperation || !InventoryComponent) return false;

	AItemBase* Payload = Cast<AItemBase>(InOperation->Payload);
	if (!Payload) return false;

	DraggingItem = Payload;

	FVector2D MouseScreenPos = InDragDropEvent.GetScreenSpacePosition();
	FVector2D LocalPos = InGeometry.AbsoluteToLocal(MouseScreenPos);
	FVector2D GridOrigin = GridBorder->GetCachedGeometry().GetLocalPositionAtCoordinates(FVector2D(0.0f, 0.0f));
	FVector2D RelativePos = LocalPos - GridOrigin;

	FIntPoint Dims = Payload->GetDimensions();
	float OffsetX = FMath::FloorToInt(Dims.X * 0.5f) * TileSize;
	float OffsetY = FMath::FloorToInt(Dims.Y * 0.5f) * TileSize;

	int32 TileX = FMath::FloorToInt((RelativePos.X - OffsetX) / TileSize);
	int32 TileY = FMath::FloorToInt((RelativePos.Y - OffsetY) / TileSize);

	CurrentDragTile = FIntPoint(TileX, TileY);

	int32 TargetIndex = InventoryComponent->TileToIndex(CurrentDragTile);
	bCanDrop = InventoryComponent->IsRoomAvailableForPayload(Payload, TargetIndex, Payload);

	Invalidate(EInvalidateWidgetReason::Paint);

	return true;
}

bool UInventoryGridWidget::NativeOnDrop(
	const FGeometry& InGeometry,
	const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	if (!InOperation || !InventoryComponent) return false;

	AItemBase* Payload = Cast<AItemBase>(InOperation->Payload);
	if (!Payload) return false;

	int32 TargetIndex = InventoryComponent->TileToIndex(CurrentDragTile);
	if (InventoryComponent->IsRoomAvailableForPayload(Payload, TargetIndex, Payload))
	{
		InventoryComponent->RefreshAllItems();
		InventoryComponent->AddItemAt(Payload, TargetIndex);
	}
	else
	{
		bool bPlaced = false;
		for (int32 i = 0; i < InventoryComponent->Items.Num(); i++)
		{
			if (InventoryComponent->IsRoomAvailableForPayload(Payload, i, Payload))
			{
				InventoryComponent->RefreshAllItems();
				InventoryComponent->AddItemAt(Payload, i);
				bPlaced = true;
				break;
			}
		}

		if (!bPlaced && CharacterReference)
		{
			FVector DropLocation = CharacterReference->GetActorLocation()
				+ CharacterReference->GetActorForwardVector() * 150.0f;
			Payload->SetActorLocation(DropLocation);
		}
	}

	DraggingItem = nullptr;
	CurrentDragTile = FIntPoint(-1, -1);
	bCanDrop = false;

	if (CharacterReference)
	{
		CharacterReference->DraggingItem = nullptr;
	}

	Invalidate(EInvalidateWidgetReason::Paint);
	return true;
}

FReply UInventoryGridWidget::NativeOnPreviewKeyDown(
	const FGeometry& InGeometry,
	const FKeyEvent& InKeyEvent)
{
	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

void UInventoryGridWidget::Refresh()
{
	GridCanvasPanel->ClearChildren();

	TMap<AItemBase*, FIntPoint> AllItems = InventoryComponent->GetAllItems();

	if (!CharacterReference->ItemWidgetClass) return;

	for (auto& Pair : AllItems)
	{
		AItemBase* CurrentItem = Pair.Key;
		FIntPoint  TopLeftTile = Pair.Value;

		if (!CurrentItem) continue;

		CharacterReference->ItemToAdd = CurrentItem;

		UUserWidget* NewItemWidget = CreateWidget(GetWorld(), CharacterReference->ItemWidgetClass);
		NewItemWidget->SetOwningPlayer(GetOwningPlayer());

		int32 X = TopLeftTile.X * InventoryComponent->TileSize;
		int32 Y = TopLeftTile.Y * InventoryComponent->TileSize;

		UPanelSlot* NewSlot = GridCanvasPanel->AddChild(NewItemWidget);
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(NewSlot);
		if (CanvasSlot)
		{
			CanvasSlot->SetAutoSize(true);
			CanvasSlot->SetPosition(FVector2D(X, Y));
		}
	}

	if (CharacterReference)
	{
		CharacterReference->NotifyInventoryVisualsChanged();
	}
}