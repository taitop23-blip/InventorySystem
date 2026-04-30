// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UCanvasPanel;
class UBorder;
class UBackgroundBlur;
class UTextBlock;
class AIntentorySystemCppCharacter;

UCLASS()
class INTENTORYSYSTEMCPP_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void RefreshDetailsFromCharacter(AIntentorySystemCppCharacter* Character);

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UCanvasPanel* Canvas;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UBorder* BackgroundBorder;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UBackgroundBlur* Blur;

	UPROPERTY(VisibleAnywhere, meta = (BindWidgetOptional), Category = "UI")
	UTextBlock* ItemDetailsText;

	UPROPERTY(VisibleAnywhere, meta = (BindWidgetOptional), Category = "UI")
	UTextBlock* EarnedTitlesText;

protected:

	virtual void NativeConstruct() override;

	// [01:14] ?��?? ?? ???? ??? ???? ? ???�J ????? ???????? ???
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	// [36:18] ???? ???????? ???? ????
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
