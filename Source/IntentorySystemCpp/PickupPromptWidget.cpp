// Fill out your copyright notice in the Description page of Project Settings.
#include "PickupPromptWidget.h"
#include "Components/TextBlock.h"

void UPickupPromptWidget::NativeConstruct()
{
	Super::NativeConstruct();
	HidePrompt();
}

void UPickupPromptWidget::ShowPrompt(const FString& ItemName)
{
	if (!PromptText) return;

	// "[ F ] AK47 줍기" 형태로 표시
	FString Message = FString::Printf(TEXT("[ F ]  %s 줍기"), *ItemName);
	PromptText->SetText(FText::FromString(Message));
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UPickupPromptWidget::HidePrompt()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
