#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class INTENTORYSYSTEMCPP_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Detection")
	float DetectionInterval = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Detection")
	float DetectionRange = 3000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Detection")
	FVector EyeOffset = FVector(0.f, 0.f, 60.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Detection")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Detection")
	ACharacter* DetectedCharacter = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Movement")
	float ChaseSpeed = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Movement")
	float StopDistance = 120.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Health")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Health")
	float CurrentHealth = 100.f;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void RequestDetectionTrace();
	void OnDetectionTraceComplete(const FTraceHandle& TraceHandle, FTraceDatum& TraceDatum);

	FTimerHandle DetectionTimerHandle;
	FTraceDelegate DetectionTraceDelegate;
	FTraceHandle ActiveTraceHandle;

	UPROPERTY()
	ACharacter* TraceTarget = nullptr;

	bool bTraceInFlight = false;
};
