// Fill out your copyright notice in the Description page of Project Settings.
// InventoryGridWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "InventoryDataStructs.h"
#include "InventoryGridWidget.generated.h"

class UCanvasPanel;
class UBorder;
class AIntentorySystemCppCharacter;
class UInventoryComponent;
class AItemBase;

UCLASS()
class INTENTORYSYSTEMCPP_API UInventoryGridWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UCanvasPanel* Canvas;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UBorder* GridBorder;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UCanvasPanel* GridCanvasPanel;

	AIntentorySystemCppCharacter* CharacterReference;
	UInventoryComponent* InventoryComponent;

	int32 Columns;
	int32 Rows;
	float TileSize;

	TArray<float> VertStartX, VertStartY, VertEndX, VertEndY;
	TArray<float> HorzStartX, HorzStartY, HorzEndX, HorzEndY;

	FIntPoint CurrentDragTile = FIntPoint(-1, -1);

	AItemBase* DraggingItem = nullptr;

	bool bCanDrop = false;

	virtual void NativeConstruct() override;
	void CreateLineSegments();

	virtual int32 NativePaint(const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;

	virtual bool NativeOnDragOver(const FGeometry& InGeometry,
		const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation) override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry,
		const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation) override;

	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry,
		const FKeyEvent& InKeyEvent) override;

public:
	void Refresh();

	void RequestRepaint() { Invalidate(EInvalidateWidgetReason::Paint); }
};