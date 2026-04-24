// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PickupPromptWidget.generated.h"

class UTextBlock;

UCLASS()
class INTENTORYSYSTEMCPP_API UPickupPromptWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 프롬프트 텍스트를 외부에서 설정할 수 있도록 노출
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowPrompt(const FString& ItemName);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HidePrompt();

protected:
	// BP에서 BindWidget으로 연결할 텍스트 블록
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UTextBlock* PromptText;

	virtual void NativeConstruct() override;
};
