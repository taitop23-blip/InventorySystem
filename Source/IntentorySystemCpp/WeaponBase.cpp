#include "WeaponBase.h"

#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"

AWeaponBase::AWeaponBase()
{
	ItemName = TEXT("Weapon");
	ItemDescription = TEXT("Base weapon item.");
}

void AWeaponBase::Fire(ACharacter* WeaponOwner)
{
	if (!WeaponOwner || !CanFire())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FVector ViewLocation = WeaponOwner->GetActorLocation();
	FRotator ViewRotation = WeaponOwner->GetActorRotation();
	if (AController* OwnerController = WeaponOwner->GetController())
	{
		OwnerController->GetPlayerViewPoint(ViewLocation, ViewRotation);
	}

	const FVector OwnerLocation = WeaponOwner->GetActorLocation();
	const FVector Start = OwnerLocation
		+ WeaponOwner->GetActorForwardVector() * MuzzleOffset.X
		+ WeaponOwner->GetActorRightVector() * MuzzleOffset.Y
		+ WeaponOwner->GetActorUpVector() * MuzzleOffset.Z;
	const FVector ForwardDirection = ViewRotation.Vector();
	const float SpreadRadians = FMath::DegreesToRadians(SpreadAngle);
	const int32 SafePelletCount = FMath::Max(1, PelletCount);

	for (int32 PelletIndex = 0; PelletIndex < SafePelletCount; ++PelletIndex)
	{
		const FVector Direction = SpreadRadians > 0.f
			? FMath::VRandCone(ForwardDirection, SpreadRadians)
			: ForwardDirection;
		FirePellet(WeaponOwner, Start, Direction);
	}

	ApplyRecoil(WeaponOwner);
	LastFireTime = World->GetTimeSeconds();
}

bool AWeaponBase::CanFire() const
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	return World->GetTimeSeconds() - LastFireTime >= FireRate;
}

void AWeaponBase::ApplyRecoil(ACharacter* WeaponOwner) const
{
	if (!WeaponOwner)
	{
		return;
	}

	const float RandomYaw = FMath::RandRange(-RecoilYaw, RecoilYaw);
	WeaponOwner->AddControllerPitchInput(-RecoilPitch);
	WeaponOwner->AddControllerYawInput(RandomYaw);
}

void AWeaponBase::FirePellet(ACharacter* WeaponOwner, const FVector& Start, const FVector& Direction)
{
	UWorld* World = GetWorld();
	if (!World || !WeaponOwner)
	{
		return;
	}

	const FVector End = Start + Direction * Range;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(WeaponFire), true);
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(WeaponOwner);

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	FHitResult HitResult;
	const bool bHit = World->LineTraceSingleByObjectType(HitResult, Start, End, ObjectQueryParams, QueryParams);
	const FVector TraceEnd = bHit ? HitResult.ImpactPoint : End;

	DrawDebugLine(World, Start, TraceEnd, bHit ? FColor::Red : FColor::Green, false, 1.5f, 0, 1.5f);

	if (bHit && HitResult.GetActor())
	{
		UE_LOG(LogTemp, Log, TEXT("%s hit %s for %.1f damage."),
			*GetName(),
			*GetNameSafe(HitResult.GetActor()),
			Damage);

		UGameplayStatics::ApplyPointDamage(
			HitResult.GetActor(),
			Damage,
			Direction,
			HitResult,
			WeaponOwner->GetController(),
			this,
			nullptr);
	}
}
