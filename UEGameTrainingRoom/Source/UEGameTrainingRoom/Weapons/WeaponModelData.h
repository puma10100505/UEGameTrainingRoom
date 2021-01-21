#pragma once

#include "CoreMinimal.h"

#include "WeaponModelData.generated.h"

USTRUCT(BlueprintType)
struct FWeaponModelData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 ClipCapacity; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 MaxAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName MuzzleName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float FireInterval;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float ShootRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 BulletNumPerShoot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class USoundBase* MuzzleSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UParticleSystem* MuzzleFlash;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UParticleSystem* ImpactFlash;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class TSubclassOf<AActor> GunTrail;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class USoundBase* ImpactSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float AimingFOV;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DefaultFOV;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float AimingSpeed;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float ReloadTime;
};