// Copyright Epic Games, Inc. All Rights Reserved.
// IntentorySystemCppCharacter.cpp
#include "IntentorySystemCppCharacter.h"
#include "IntentorySystemCpp.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "InventoryComponent.h"
#include "InventoryGridWidget.h"
#include "ItemBase.h"
#include "ItemWidget.h"
#include "PickupPromptWidget.h"
#include "InventoryWidget.h"
#include "WeaponBase.h"
#include "MyActorComponent.h"
#include "HealthBarWidget.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AIntentorySystemCppCharacter::AIntentorySystemCppCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	HealthComponent = CreateDefaultSubobject<UMyActorComponent>(TEXT("HealthComponent"));

	// BeginOverlap: 범위 안에 들어온 아이템을 OverlappingItem으로 기억
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(
		this, &AIntentorySystemCppCharacter::OnBeginOverlap);

	// EndOverlap: 범위에서 벗어나면 OverlappingItem 초기화
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(
		this, &AIntentorySystemCppCharacter::OnEndOverlap);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void AIntentorySystemCppCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (CameraBoom)
	{
		DefaultCameraArmLength = CameraBoom->TargetArmLength;
		DefaultCameraSocketOffset = CameraBoom->SocketOffset;
	}

	GetPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (GetPlayerController)
	{
		if (InventoryWidgetClass)
		{
			InventoryWidget = CreateWidget(GetWorld(), InventoryWidgetClass);
			InventoryWidget->SetOwningPlayer(GetPlayerController);
			InventoryWidget->AddToViewport();
			InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
		}

		// 픽업 프롬프트 위젯 생성
		if (PickupPromptWidgetClass)
		{
			PickupPromptWidget = CreateWidget<UPickupPromptWidget>(GetWorld(), PickupPromptWidgetClass);
			PickupPromptWidget->SetOwningPlayer(GetPlayerController);
			PickupPromptWidget->AddToViewport();
			PickupPromptWidget->HidePrompt();
		}
	}

	InventoryComponent->Items.SetNum(InventoryComponent->Columns * InventoryComponent->Rows);

	if (HealthBarWidgetClass && GetPlayerController)
	{
		HealthBarWidget = CreateWidget<UHealthBarWidget>(GetWorld(), HealthBarWidgetClass);
		if (HealthBarWidget)
		{
			HealthBarWidget->SetOwningPlayer(GetPlayerController);
			HealthBarWidget->AddToViewport();
		}
	}

	if (HealthComponent)
	{
		HealthComponent->OnHealthDead.AddDynamic(this, &AIntentorySystemCppCharacter::HandleCharacterDead);
		HealthComponent->OnHealthDamaged.AddDynamic(this, &AIntentorySystemCppCharacter::HandleHealthDamaged);
	}

	UE_LOG(LogIntentoryAssignment, Log, TEXT("PIE/Game: Character ready (%s). G=earn title, F=pickup."), *GetNameSafe(this));
}

void AIntentorySystemCppCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AIntentorySystemCppCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AIntentorySystemCppCharacter::Move);
		EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &AIntentorySystemCppCharacter::Look);
		EIC->BindAction(InventoryAction, ETriggerEvent::Started, this, &AIntentorySystemCppCharacter::ToggleInventory);

		if (RotateItemAction)
			EIC->BindAction(RotateItemAction, ETriggerEvent::Started, this, &AIntentorySystemCppCharacter::RotateDraggingItem);

		// F키 픽업 바인딩
		if (PickupAction)
			EIC->BindAction(PickupAction, ETriggerEvent::Started, this, &AIntentorySystemCppCharacter::PickupItem);

		// Optional extra key (IMC). G is also bound below — do not map the same IA to G or titles may advance twice per press.
		if (GrantTitleAction)
			EIC->BindAction(GrantTitleAction, ETriggerEvent::Started, this, &AIntentorySystemCppCharacter::GrantNextDemoTitle);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find Enhanced Input component!"), *GetNameSafe(this));
	}

	// G: 칭호 획득 (데모 순서: 무기 전문가 → 근접 달인 → 폭발물 면허)
	PlayerInputComponent->BindKey(EKeys::G, IE_Pressed, this, &AIntentorySystemCppCharacter::GrantNextDemoTitle);
	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AIntentorySystemCppCharacter::FireCurrentWeapon);
	PlayerInputComponent->BindKey(EKeys::One, IE_Pressed, this, &AIntentorySystemCppCharacter::EquipWeaponSlot1);
	PlayerInputComponent->BindKey(EKeys::Two, IE_Pressed, this, &AIntentorySystemCppCharacter::EquipWeaponSlot2);
	PlayerInputComponent->BindKey(EKeys::NumPadOne, IE_Pressed, this, &AIntentorySystemCppCharacter::EquipWeaponSlot1);
	PlayerInputComponent->BindKey(EKeys::NumPadTwo, IE_Pressed, this, &AIntentorySystemCppCharacter::EquipWeaponSlot2);

	PlayerInputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &AIntentorySystemCppCharacter::StartAiming);
	PlayerInputComponent->BindKey(EKeys::RightMouseButton, IE_Released, this, &AIntentorySystemCppCharacter::StopAiming);
}

void AIntentorySystemCppCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector  ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector  RightDir   = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(ForwardDir, MovementVector.Y);
		AddMovementInput(RightDir, MovementVector.X);
	}
}

void AIntentorySystemCppCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AIntentorySystemCppCharacter::ToggleInventory()
{
	if (!InventoryWidget || !GetPlayerController) return;

	if (InventoryWidget->GetVisibility() == ESlateVisibility::Collapsed)
	{
		StopAiming();
		InventoryWidget->SetVisibility(ESlateVisibility::Visible);
		GetPlayerController->SetInputMode(FInputModeGameAndUI());
		GetPlayerController->SetShowMouseCursor(true);
		NotifyInventoryVisualsChanged();
	}
	else
	{
		InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
		GetPlayerController->SetInputMode(FInputModeGameOnly());
		GetPlayerController->SetShowMouseCursor(false);
	}
}

void AIntentorySystemCppCharacter::RotateDraggingItem()
{
	if (!DraggingItem) return;

	DraggingItem->Rotate();

	if (DraggingItemWidget)
		DraggingItemWidget->Refresh(DraggingItem);

	if (InventoryComponent && InventoryComponent->InventoryGridWidgetReference)
		InventoryComponent->InventoryGridWidgetReference->RequestRepaint();
}

// F키를 눌렀을 때 범위 안의 아이템을 인벤토리에 추가
void AIntentorySystemCppCharacter::PickupItem()
{
	if (!OverlappingItem) return;

	// 이미 파괴된 액터인지 체크
	if (!IsValid(OverlappingItem))
	{
		OverlappingItem = nullptr;
		if (PickupPromptWidget) PickupPromptWidget->HidePrompt();
		return;
	}

	if (!MeetsItemTitleRequirement(OverlappingItem))
	{
		UE_LOG(LogIntentoryAssignment, Warning, TEXT("Pickup blocked: item '%s' needs title '%s' (press G first)."),
			*OverlappingItem->ItemName, *OverlappingItem->RequiredTitleToUse);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.8f, FColor::Orange,
				TEXT("\uBA3C\uC800 G\uB97C \uB20C\uB7EC \uD544\uC694\uD55C \uCE6D\uD638\uB97C \uD68D\uB4DD\uD55C \uD6C4 F\uB85C \uC8FC\uC138\uC694."));
		}
		return;
	}

	if (InventoryComponent->TryAddItem(OverlappingItem))
	{
		const FString PickedName = OverlappingItem->ItemName;
		UE_LOG(LogIntentoryAssignment, Log, TEXT("Pickup OK: '%s' stored in TArray bag grid."), *PickedName);

		if (AWeaponBase* PickedWeapon = Cast<AWeaponBase>(OverlappingItem))
		{
			if (!OwnedWeapons.Contains(PickedWeapon))
			{
				OwnedWeapons.Add(PickedWeapon);
			}
			PickedWeapon->SetActorHiddenInGame(true);
			PickedWeapon->SetActorEnableCollision(false);
			UE_LOG(LogIntentoryAssignment, Log, TEXT("Stored weapon slot %d: '%s'. Press 1/2 to equip by pickup order."),
				OwnedWeapons.IndexOfByKey(PickedWeapon) + 1,
				*PickedName);
		}
		else
		{
			OverlappingItem->Destroy();
		}
		OverlappingItem = nullptr;

		// 줍기 성공 시 프롬프트 숨기기
		if (PickupPromptWidget)
			PickupPromptWidget->HidePrompt();

		NotifyInventoryVisualsChanged();
	}
	else
	{
		UE_LOG(LogIntentoryAssignment, Warning, TEXT("Pickup failed: no room for '%s'."), *OverlappingItem->ItemName);
	}
}

void AIntentorySystemCppCharacter::FireCurrentWeapon()
{
	if (!CurrentWeapon || !IsValid(CurrentWeapon))
	{
		UE_LOG(LogIntentoryAssignment, Warning, TEXT("No current weapon. Pick up AK-47 or Shotgun first."));
		return;
	}

	CurrentWeapon->Fire(this);
}

void AIntentorySystemCppCharacter::EquipWeaponSlot1()
{
	EquipWeaponBySlot(0);
}

void AIntentorySystemCppCharacter::EquipWeaponSlot2()
{
	EquipWeaponBySlot(1);
}

void AIntentorySystemCppCharacter::EquipWeaponBySlot(int32 SlotIndex)
{
	CleanupOwnedWeapons();

	if (!OwnedWeapons.IsValidIndex(SlotIndex))
	{
		UE_LOG(LogIntentoryAssignment, Warning, TEXT("Weapon slot %d is empty."), SlotIndex + 1);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Yellow,
				FString::Printf(TEXT("Weapon slot %d is empty."), SlotIndex + 1));
		}
		return;
	}

	AWeaponBase* WeaponToEquip = OwnedWeapons[SlotIndex];
	if (!WeaponToEquip || !IsValid(WeaponToEquip))
	{
		return;
	}

	AWeaponBase* PreviouslyEquippedWeapon = CurrentWeapon;

	if (IsWeaponInInventory(WeaponToEquip))
	{
		InventoryComponent->RemoveItem(WeaponToEquip);
	}

	if (PreviouslyEquippedWeapon && PreviouslyEquippedWeapon != WeaponToEquip && IsValid(PreviouslyEquippedWeapon) && !IsWeaponInInventory(PreviouslyEquippedWeapon))
	{
		if (!InventoryComponent->TryAddItem(PreviouslyEquippedWeapon))
		{
			UE_LOG(LogIntentoryAssignment, Warning, TEXT("Could not return '%s' to inventory while switching weapons."),
				*PreviouslyEquippedWeapon->ItemName);
		}
	}

	CurrentWeapon = WeaponToEquip;
	if (CurrentWeapon)
	{
		CurrentWeapon->SetActorHiddenInGame(true);
		CurrentWeapon->SetActorEnableCollision(false);
		UE_LOG(LogIntentoryAssignment, Log, TEXT("Equipped slot %d weapon: '%s'."), SlotIndex + 1, *CurrentWeapon->ItemName);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green,
				FString::Printf(TEXT("Equipped %d: %s"), SlotIndex + 1, *CurrentWeapon->ItemName));
		}
	}

	RefreshWeaponInventoryUI();
}

bool AIntentorySystemCppCharacter::IsWeaponInInventory(AWeaponBase* Weapon) const
{
	if (!Weapon || !InventoryComponent)
	{
		return false;
	}

	return InventoryComponent->Items.Contains(Weapon);
}

void AIntentorySystemCppCharacter::CleanupOwnedWeapons()
{
	for (int32 Index = OwnedWeapons.Num() - 1; Index >= 0; --Index)
	{
		if (!IsValid(OwnedWeapons[Index]))
		{
			if (CurrentWeapon == OwnedWeapons[Index])
			{
				CurrentWeapon = nullptr;
			}
			OwnedWeapons.RemoveAt(Index);
		}
	}
}

void AIntentorySystemCppCharacter::RefreshWeaponInventoryUI()
{
	if (InventoryComponent && InventoryComponent->InventoryGridWidgetReference)
	{
		InventoryComponent->InventoryGridWidgetReference->Refresh();
	}

	NotifyInventoryVisualsChanged();
}

bool AIntentorySystemCppCharacter::HasEarnedTitle(const FString& Title) const
{
	return EarnedTitles.Contains(Title);
}

bool AIntentorySystemCppCharacter::MeetsItemTitleRequirement(AItemBase* Item) const
{
	if (!Item)
	{
		return false;
	}
	if (Item->RequiredTitleToUse.IsEmpty())
	{
		return true;
	}
	return EarnedTitles.Contains(Item->RequiredTitleToUse);
}

bool AIntentorySystemCppCharacter::CanTakeItemOutToWorld(AItemBase* Item) const
{
	return MeetsItemTitleRequirement(Item);
}

void AIntentorySystemCppCharacter::GrantTitle(const FString& Title)
{
	if (Title.IsEmpty())
	{
		return;
	}
	EarnedTitles.Add(Title);
	UE_LOG(LogIntentoryAssignment, Log, TEXT("Title earned (TSet): '%s' | count=%d"), *Title, EarnedTitles.Num());
	NotifyInventoryVisualsChanged();
}

void AIntentorySystemCppCharacter::NotifyInventoryVisualsChanged()
{
	if (UInventoryWidget* Panel = Cast<UInventoryWidget>(InventoryWidget))
	{
		Panel->RefreshDetailsFromCharacter(this);
	}
}

void AIntentorySystemCppCharacter::GrantNextDemoTitle()
{
	static const TArray<FString> DemoOrder = {
		TEXT("\uBB34\uAE30 \uC804\uBB38\uAC00"),
		TEXT("\uADFC\uC811 \uB2EC\uC778"),
		TEXT("\uD3ED\uBC1C\uBB3C \uBA74\uD5C8")
	};

	for (const FString& Title : DemoOrder)
	{
		if (!EarnedTitles.Contains(Title))
		{
			GrantTitle(Title);
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Cyan,
					FString::Printf(TEXT("\uCE6D\uD638 \uD68D\uB4DD: %s"), *Title));
			}
			return;
		}
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
			TEXT("\uB370\uBAA8 \uCE6D\uD638\uB97C \uBAA8\uB450 \uBCF4\uC720 \uC911\uC785\uB2C8\uB2E4."));
	}
	UE_LOG(LogIntentoryAssignment, Log, TEXT("GrantTitle(G): all demo titles already in TSet (no change)."));
}

// 캡슐 범위에 아이템이 들어오면 OverlappingItem으로 기억 (자동 줍기 제거)
void AIntentorySystemCppCharacter::OnBeginOverlap(
	UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	AItemBase* Item = Cast<AItemBase>(OtherActor);
	if (Item)
	{
		OverlappingItem = Item;
		ItemToAdd = OtherActor;

		// 픽업 프롬프트 표시
		if (PickupPromptWidget)
			PickupPromptWidget->ShowPrompt(Item->ItemName);
	}
}

// 캡슐 범위에서 아이템이 벗어나면 OverlappingItem 초기화
void AIntentorySystemCppCharacter::OnEndOverlap(
	UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherActor == OverlappingItem)
	{
		OverlappingItem = nullptr;
		ItemToAdd = nullptr;

		// 픽업 프롬프트 숨기기
		if (PickupPromptWidget)
			PickupPromptWidget->HidePrompt();
	}
}

void AIntentorySystemCppCharacter::HandleCharacterDead(AController* InstigatorController)
{
	StopAiming();

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement();
	}

	if (USkeletalMeshComponent* SK = GetMesh())
	{
		SK->SetCollisionProfileName(TEXT("Ragdoll"));
		SK->SetSimulatePhysics(true);
	}

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}

	if (InventoryComponent)
	{
		for (AItemBase*& SlotItem : InventoryComponent->Items)
		{
			SlotItem = nullptr;
		}
		InventoryComponent->AddedItem = true;
	}

	OwnedWeapons.Empty();
	CurrentWeapon = nullptr;
	NotifyInventoryVisualsChanged();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("You Died"));
	}

	UE_LOG(LogIntentoryAssignment, Log, TEXT("Character dead: ragdoll + input disabled."));
}

void AIntentorySystemCppCharacter::HandleHealthDamaged(float NewHealth, float MaxHealth, float HealthChange)
{
	if (HealthBarWidget)
	{
		HealthBarWidget->UpdateHP(NewHealth, MaxHealth, HealthChange);
	}
}

void AIntentorySystemCppCharacter::StartAiming()
{
	if (bIsAiming || !Controller)
	{
		return;
	}

	bIsAiming = true;

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->bOrientRotationToMovement = false;
	}
	bUseControllerRotationYaw = true;

	if (CameraBoom)
	{
		CameraBoom->TargetArmLength = AimedArmLength;
		CameraBoom->SocketOffset = AimedSocketOffset;
	}
}

void AIntentorySystemCppCharacter::StopAiming()
{
	if (!bIsAiming)
	{
		return;
	}

	bIsAiming = false;

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->bOrientRotationToMovement = true;
	}
	bUseControllerRotationYaw = false;

	if (CameraBoom)
	{
		CameraBoom->TargetArmLength = DefaultCameraArmLength;
		CameraBoom->SocketOffset = DefaultCameraSocketOffset;
	}
}
