// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryWidget.h"
#include "IntentorySystemCpp.h"
#include "IntentorySystemCppCharacter.h"
#include "ItemBase.h"
#include "InventoryComponent.h"
#include "InventoryGridWidget.h"
#include "InventoryDataStructs.h"
#include "Components/TextBlock.h"
#include "Blueprint/DragDropOperation.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (AIntentorySystemCppCharacter* Character = Cast<AIntentorySystemCppCharacter>(
		UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		RefreshDetailsFromCharacter(Character);
	}
}

void UInventoryWidget::RefreshDetailsFromCharacter(AIntentorySystemCppCharacter* Character)
{
	if (!Character || !Character->InventoryComponent)
	{
		return;
	}

	Character->InventoryComponent->GetAllItems();

	if (ItemDetailsText)
	{
		FString Body = TEXT("=== Bag items (TMap from TArray) ===\n");
		for (const TPair<FString, FItemDetailInfo>& Pair : Character->InventoryComponent->ItemInfoByName)
		{
			const FItemDetailInfo& Info = Pair.Value;
			const FString Req = Info.RequiredTitle.IsEmpty()
				? TEXT("(none)")
				: Info.RequiredTitle;
			Body += FString::Printf(
				TEXT("[%s]\n%s\nRequired title: %s\nSize: %dx%d\n\n"),
				*Info.DisplayName,
				*Info.Description,
				*Req,
				Info.Dimensions.X,
				Info.Dimensions.Y);
		}
		if (Character->InventoryComponent->ItemInfoByName.Num() == 0)
		{
			Body += TEXT("(empty)\n");
		}
		ItemDetailsText->SetText(FText::FromString(Body));
	}

	UE_LOG(LogIntentoryAssignment, Log, TEXT("UI: TMap ItemInfoByName entries=%d | TSet earned titles=%d."),
		Character->InventoryComponent->ItemInfoByName.Num(), Character->EarnedTitles.Num());

	if (EarnedTitlesText)
	{
		FString TitlesLine = TEXT("Earned titles (TSet): ");
		if (Character->EarnedTitles.Num() == 0)
		{
			TitlesLine += TEXT("(none)");
		}
		else
		{
			bool bFirst = true;
			for (const FString& Title : Character->EarnedTitles)
			{
				if (!bFirst)
				{
					TitlesLine += TEXT(", ");
				}
				bFirst = false;
				TitlesLine += Title;
			}
		}
		EarnedTitlesText->SetText(FText::FromString(TitlesLine));
	}
}

FReply UInventoryWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return FReply::Handled();
}

bool UInventoryWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (!InOperation)
		return false;

	AItemBase* DroppedItem = Cast<AItemBase>(InOperation->Payload);
	if (!DroppedItem)
		return false;

	AIntentorySystemCppCharacter* Character = Cast<AIntentorySystemCppCharacter>(
		UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!Character)
		return false;

	Character->DraggingItem = nullptr;
	Character->DraggingItemWidget = nullptr;

	if (!Character->CanTakeItemOutToWorld(DroppedItem))
	{
		UE_LOG(LogIntentoryAssignment, Warning, TEXT("Drop-to-world blocked: '%s' needs title '%s'."),
			*DroppedItem->ItemName, *DroppedItem->RequiredTitleToUse);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
				TEXT("Missing required title to use this item. Press your GrantTitle input to earn demo titles."));
		}

		if (Character->InventoryComponent && Character->InventoryComponent->TryAddItem(DroppedItem))
		{
			if (Character->InventoryComponent->InventoryGridWidgetReference)
			{
				Character->InventoryComponent->InventoryGridWidgetReference->Refresh();
			}
			Character->NotifyInventoryVisualsChanged();
		}
		else
		{
			const FVector Fallback = Character->GetActorLocation()
				+ Character->GetActorForwardVector() * 120.f;
			DroppedItem->SetActorLocation(Fallback);
		}

		return true;
	}

	FRotator SpawnRotation = Character->GetActorRotation();

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);

	FVector TraceStart = Character->GetActorLocation()
		+ Character->GetActorForwardVector() * 100.f;
	FVector TraceEnd = TraceStart + FVector(0.f, 0.f, -1000.f);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		ECC_Visibility,
		Params
	);

	FVector SpawnLocation = bHit ? HitResult.ImpactPoint : TraceStart;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	GetWorld()->SpawnActor<AItemBase>(
		DroppedItem->GetClass(),
		SpawnLocation,
		SpawnRotation,
		SpawnParams);

	UE_LOG(LogIntentoryAssignment, Log, TEXT("Drop-to-world OK: spawned '%s' (payload destroyed)."), *DroppedItem->ItemName);

	DroppedItem->Destroy();

	Character->NotifyInventoryVisualsChanged();

	return true;
}