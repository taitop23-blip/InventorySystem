// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemWidget.h"
#include "IntentorySystemCppCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "ItemBase.h"
#include "InventoryComponent.h"
#include "InventoryGridWidget.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/SizeBox.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FLinearColor InitialColor = BackgroundBorder->GetBrushColor();
	InitialColor.A = 0.0f;
	BackgroundBorder->SetBrushColor(InitialColor);

	CharacterReference = Cast<AIntentorySystemCppCharacter>(
		UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (CharacterReference)
	{
		AItemBase* Item = Cast<AItemBase>(CharacterReference->ItemToAdd);
		Refresh(Item);
	}
}

void UItemWidget::Refresh(AItemBase* ItemToDisplay)
{
	if (!ItemToDisplay) return;
	if (!CharacterReference || !CharacterReference->InventoryComponent) return;

	RepresentedItem = ItemToDisplay;

	if (ItemToDisplay->GetIcon())
	{
		ItemImage->SetBrushFromMaterial(ItemToDisplay->GetIcon());
	}

	float TileSize = CharacterReference->InventoryComponent->TileSize;
	Size = FVector2D(
		ItemToDisplay->GetDimensions().X * TileSize,
		ItemToDisplay->GetDimensions().Y * TileSize
	);

	BackgroundSizeBox->SetWidthOverride(Size.X);
	BackgroundSizeBox->SetHeightOverride(Size.Y);

	UCanvasPanelSlot* ImageAsCanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(ItemImage);
	if (ImageAsCanvasSlot)
	{
		ImageAsCanvasSlot->SetSize(Size);
	}
}

void UItemWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	FLinearColor HoverColor = BackgroundBorder->GetBrushColor();
	HoverColor.A = 1.0f;
	BackgroundBorder->SetBrushColor(HoverColor);
}

void UItemWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	FLinearColor NormalColor = BackgroundBorder->GetBrushColor();
	NormalColor.A = 0.0f;
	BackgroundBorder->SetBrushColor(NormalColor);
}

FReply UItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}
	return FReply::Unhandled();
}

void UItemWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (!RepresentedItem || !CharacterReference) return;

	UDragDropOperation* DragOp = NewObject<UDragDropOperation>(UDragDropOperation::StaticClass());
	DragOp->Payload = RepresentedItem;
	DragOp->DefaultDragVisual = this;
	OutOperation = DragOp;

	CharacterReference->DraggingItem = RepresentedItem;
	CharacterReference->DraggingItemWidget = this;

	CharacterReference->InventoryComponent->RemoveItem(RepresentedItem);
	RemoveFromParent();
}