// Copyright Epic Games, Inc. All Rights Reserved.
// IntentorySystemCppCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "IntentorySystemCppCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UInventoryComponent;
class AItemBase;
class AWeaponBase;
class UItemWidget;
class UPickupPromptWidget;
class UMyActorComponent;
class UHealthBarWidget;
class UAimCrosshairWidget;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game)
class AIntentorySystemCppCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InventoryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RotateItemAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PickupAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* GrantTitleAction;

public:
	AIntentorySystemCppCharacter();

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(EditAnywhere)
	UInventoryComponent* InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	UMyActorComponent* HealthComponent;

	/** Assignment: earned titles (TSet). Required to take some items out of the bag to the world. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory|Titles")
	TSet<FString> EarnedTitles;

	bool HasEarnedTitle(const FString& Title) const;

	/** Item requires no title, or player has RequiredTitleToUse in EarnedTitles. Used for pickup and drop-to-world. */
	bool MeetsItemTitleRequirement(AItemBase* Item) const;

	bool CanTakeItemOutToWorld(AItemBase* Item) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Titles")
	void GrantTitle(const FString& Title);

	void NotifyInventoryVisualsChanged();

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> ItemWidgetClass;
	UUserWidget* ItemWidget;

	AActor* ItemToAdd;

	AItemBase* DraggingItem = nullptr;

	UItemWidget* DraggingItemWidget = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	AWeaponBase* CurrentWeapon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TArray<AWeaponBase*> OwnedWeapons;

protected:

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> InventoryWidgetClass;
	UUserWidget* InventoryWidget;

	// 픽업 프롬프트 위젯 (F키 안내)
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UPickupPromptWidget> PickupPromptWidgetClass;
	UPickupPromptWidget* PickupPromptWidget;

	APlayerController* GetPlayerController;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	virtual void NotifyControllerChanged() override;

	void ToggleInventory();

	void RotateDraggingItem();

	void PickupItem();

	void FireCurrentWeapon();

	void EquipWeaponSlot1();
	void EquipWeaponSlot2();
	void EquipWeaponBySlot(int32 SlotIndex);
	bool IsWeaponInInventory(AWeaponBase* Weapon) const;
	void CleanupOwnedWeapons();
	void RefreshWeaponInventoryUI();

	void GrantNextDemoTitle();

	AItemBase* OverlappingItem = nullptr;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlap(class UPrimitiveComponent* HitComp,
		class AActor* OtherActor,
		class UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(class UPrimitiveComponent* HitComp,
		class AActor* OtherActor,
		class UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	UFUNCTION()
	void HandleCharacterDead(AController* InstigatorController);

	UFUNCTION()
	void HandleHealthDamaged(float NewHealth, float MaxHealth, float HealthChange);

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UHealthBarWidget> HealthBarWidgetClass;

	UPROPERTY()
	UHealthBarWidget* HealthBarWidget;

	/** Hold right mouse to aim: crosshair + character yaw follows look direction. */
	UPROPERTY()
	UAimCrosshairWidget* AimCrosshairWidget;

	bool bIsAiming = false;

	void StartAiming();
	void StopAiming();
};