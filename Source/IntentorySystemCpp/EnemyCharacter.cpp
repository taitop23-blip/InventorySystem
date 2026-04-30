#include "EnemyCharacter.h"

#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

	GetWorldTimerManager().SetTimer(
		DetectionTimerHandle,
		this,
		&AEnemyCharacter::RequestDetectionTrace,
		DetectionInterval,
		true,
		0.f);
}

void AEnemyCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(DetectionTimerHandle);
	Super::EndPlay(EndPlayReason);
}

float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	const float DamageToApply = ActualDamage > 0.f ? ActualDamage : DamageAmount;
	if (DamageToApply <= 0.f)
	{
		return 0.f;
	}

	CurrentHealth = FMath::Max(0.f, CurrentHealth - DamageToApply);
	UE_LOG(LogTemp, Log, TEXT("%s took %.1f damage. HP: %.1f / %.1f"),
		*GetName(),
		DamageToApply,
		CurrentHealth,
		MaxHealth);

	if (CurrentHealth <= 0.f)
	{
		Destroy();
	}

	return DamageToApply;
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!DetectedCharacter || !IsValid(DetectedCharacter))
	{
		return;
	}

	FVector ToTarget = DetectedCharacter->GetActorLocation() - GetActorLocation();
	ToTarget.Z = 0.f;

	if (ToTarget.SizeSquared() <= FMath::Square(StopDistance))
	{
		return;
	}

	const FVector MoveDirection = ToTarget.GetSafeNormal();
	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
	AddMovementInput(MoveDirection, 1.f, true);

	const FRotator LookRotation = MoveDirection.Rotation();
	SetActorRotation(FRotator(0.f, LookRotation.Yaw, 0.f));
}

void AEnemyCharacter::RequestDetectionTrace()
{
	if (bTraceInFlight)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	TraceTarget = UGameplayStatics::GetPlayerCharacter(World, 0);
	if (!TraceTarget)
	{
		DetectedCharacter = nullptr;
		return;
	}

	const FVector Start = GetActorLocation() + EyeOffset;
	FVector TargetLocation = TraceTarget->GetActorLocation();
	if (const UCapsuleComponent* TargetCapsule = TraceTarget->GetCapsuleComponent())
	{
		TargetLocation.Z += TargetCapsule->GetScaledCapsuleHalfHeight() * 0.5f;
	}

	if (FVector::DistSquared(Start, TargetLocation) > FMath::Square(DetectionRange))
	{
		DetectedCharacter = nullptr;
		return;
	}

	const FVector End = TargetLocation;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(EnemyAsyncDetectionTrace), true);
	QueryParams.AddIgnoredActor(this);

	DetectionTraceDelegate.BindUObject(this, &AEnemyCharacter::OnDetectionTraceComplete);
	ActiveTraceHandle = World->AsyncLineTraceByChannel(
		EAsyncTraceType::Single,
		Start,
		End,
		TraceChannel,
		QueryParams,
		FCollisionResponseParams::DefaultResponseParam,
		&DetectionTraceDelegate);

	bTraceInFlight = true;
}

void AEnemyCharacter::OnDetectionTraceComplete(const FTraceHandle& TraceHandle, FTraceDatum& TraceDatum)
{
	bTraceInFlight = false;

	if (TraceHandle != ActiveTraceHandle || !TraceTarget)
	{
		return;
	}

	const FHitResult* FirstHit = TraceDatum.OutHits.Num() > 0 ? &TraceDatum.OutHits[0] : nullptr;
	const bool bCanSeeTarget = !FirstHit || FirstHit->GetActor() == TraceTarget;
	DetectedCharacter = bCanSeeTarget ? TraceTarget : nullptr;

	FVector DebugEnd = TraceDatum.End;
	if (FirstHit)
	{
		DebugEnd = FirstHit->ImpactPoint;
	}
	DrawDebugLine(
		GetWorld(),
		TraceDatum.Start,
		DebugEnd,
		bCanSeeTarget ? FColor::Blue : FColor::Red,
		false,
		DetectionInterval,
		0,
		2.f);

	DrawDebugSphere(
		GetWorld(),
		TraceDatum.End,
		16.f,
		12,
		bCanSeeTarget ? FColor::Blue : FColor::Yellow,
		false,
		DetectionInterval);

	if (bCanSeeTarget)
	{
		UE_LOG(LogTemp, Log, TEXT("%s detected player using async trace."), *GetName());
	}
}
