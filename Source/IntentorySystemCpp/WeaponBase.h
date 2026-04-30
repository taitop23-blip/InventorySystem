#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "WeaponBase.generated.h"

class ACharacter;

UCLASS()
class INTENTORYSYSTEMCPP_API AWeaponBase : public AItemBase
{
	GENERATED_BODY()

public:
	AWeaponBase();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Fire(ACharacter* WeaponOwner);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Fire")
	float Damage = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Fire")
	float Range = 10000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Fire")
	float FireRate = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Fire", meta = (ClampMin = "1"))
	int32 PelletCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Fire", meta = (ClampMin = "0.0"))
	float SpreadAngle = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Recoil")
	float RecoilPitch = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Recoil")
	float RecoilYaw = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Recoil")
	float RecoilRecoverySpeed = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Trace")
	FVector MuzzleOffset = FVector(60.f, 0.f, 0.f);

protected:
	virtual bool CanFire() const;
	virtual void ApplyRecoil(ACharacter* WeaponOwner) const;
	virtual void FirePellet(ACharacter* WeaponOwner, const FVector& Start, const FVector& Direction);

	float LastFireTime = -1000.f;
};
