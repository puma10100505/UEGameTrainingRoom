// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "CharacterBase.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Particles/ParticleSystem.h"


DEFINE_LOG_CATEGORY(LogWeapon);

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponBase::Fire()
{
	if (CanFire())
	{
		UE_LOG(LogWeapon, Log, TEXT("Entry of fire, LocalRole: %d, NetMode: %d"), GetLocalRole(), GetNetMode());
		if (GetLocalRole() < ROLE_Authority)
		{
			UE_LOG(LogWeapon, Log, TEXT("Before invoke fire server rpc"));

			if (GetNetMode() == ENetMode::NM_Client)
			{
				// Play Fire Sound2D
				UGameplayStatics::PlaySound2D(GetWorld(), WeaponData.MuzzleSound);

				if (IsValid(GetOwnerCharacter()))
				{
					UGameplayStatics::SpawnEmitterAttached(WeaponData.MuzzleFlash,
						GetOwnerCharacter()->GetMesh(), WeaponData.MuzzleName, 
						GetOwnerCharacter()->GetMesh()->GetSocketLocation(WeaponData.MuzzleName),
						GetOwnerCharacter()->GetMesh()->GetSocketRotation(WeaponData.MuzzleName), 
						FVector(1.0f, 1.0f, 1.0f),
						EAttachLocation::KeepWorldPosition);

					UE_LOG(LogWeapon, Log, TEXT("After emitter attach"));
				}
			}

			// Client
			ServerFire();
		}

		FireProcess();
	}
}

void AWeaponBase::FireProcess()
{
	UE_LOG(LogWeapon, Log, TEXT("Entry of fire process"));

	ConsumeAmmo();

	// 射线命中检测
	FHitResult HitInfo;
	WeaponLineTrace_Single(HitInfo);

	// Fire client VFX
	if (GetLocalRole() == ROLE_Authority)
	{
		ClientFireEffect(HitInfo);
	}
}

bool AWeaponBase::CanFire()
{
	if (CurrentAmmo <= 0) 
	{
		UE_LOG(LogWeapon, Log, TEXT("No ammo left"));
		return false;
	}

	if (IsValid(OwnerCharacter) == false)
	{
		UE_LOG(LogWeapon, Log, TEXT("Owner Character is not exist"));
		return false;
	}

	return true;
}

void AWeaponBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	InitializeWeaponData();
}

bool AWeaponBase::WeaponLineTrace_Single(FHitResult& HitInfo)
{
	if (IsValid(GetOwnerCharacter()) == false)
	{
		UE_LOG(LogWeapon, Log, TEXT("Owner character is not valid"));
		return false;
	}
	
	FVector StartPos = GetOwnerCharacter()->GetFollowCamera()->GetComponentLocation();
	FVector FireDirection = GetOwnerCharacter()->GetFollowCamera()->GetForwardVector();
	FVector EndPos = StartPos + FireDirection * WeaponData.ShootRange;

	FCollisionQueryParams QueryParams;
	QueryParams.bReturnPhysicalMaterial = true;
	bool bIsHit = GetWorld()->LineTraceSingleByChannel(HitInfo, StartPos, EndPos, ECC_Pawn, QueryParams);

	/* For Debug */
	if (bIsHit)
	{
		// Hit
		DrawDebugLine(GetWorld(), GetOwnerCharacter()->GetMesh()->GetSocketLocation(WeaponData.MuzzleName), 
			EndPos, FColor::Red, false, 1.f, ECC_WorldStatic, 1.f);
	}
	else
	{
		// Missed
		DrawDebugLine(GetWorld(), GetOwnerCharacter()->GetMesh()->GetSocketLocation(WeaponData.MuzzleName),
			EndPos, FColor::Green, false, 1.f, ECC_WorldStatic, 1.f);
	}
	/* ******** */

	return bIsHit;
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AWeaponBase, OwnerCharacter, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWeaponBase, CurrentAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWeaponBase, TotalLeftAmmo, COND_OwnerOnly);
}

void AWeaponBase::InitializeWeaponData()
{
	CurrentAmmo = WeaponData.ClipCapacity;
	TotalLeftAmmo = WeaponData.MaxAmmo;
}

void AWeaponBase::ConsumeAmmo()
{
	CurrentAmmo -= WeaponData.BulletNumPerShoot;

	if (NeedReload())
	{
		Reload();
	}
}

bool AWeaponBase::NeedReload()
{
	return CurrentAmmo <= 0;
}

void AWeaponBase::Reload()
{
	// 换弹数值变更
	int32 AmmoDiff = WeaponData.ClipCapacity - CurrentAmmo;
	if (AmmoDiff <= 0)
	{
		return;
	}

	if (AmmoDiff >= TotalLeftAmmo)
	{
		CurrentAmmo += TotalLeftAmmo;
		TotalLeftAmmo = 0;
	}
	else
	{
		CurrentAmmo = WeaponData.ClipCapacity;
		TotalLeftAmmo -= AmmoDiff;
	}
}

bool AWeaponBase::CanReload()
{
	return TotalLeftAmmo > 0;
}

void AWeaponBase::AttachTo(class ACharacterBase* NewPawn)
{
	if (NewPawn != OwnerCharacter)
	{
		OwnerCharacter = NewPawn;
		SetOwner(NewPawn);			// FOR THE RPC IMPORTANT !!!
		SetInstigator(NewPawn);
		UE_LOG(LogWeapon, Log, TEXT("After attach weapon to character and set the owner relationship"));
	}	
}

class ACharacterBase* AWeaponBase::GetOwnerCharacter() const
{
	return OwnerCharacter;
}

bool AWeaponBase::ServerFire_Validate()
{
	return true;
}

void AWeaponBase::ServerFire_Implementation()
{
	UE_LOG(LogWeapon, Log, TEXT("Server fire rpc implementation"));
	Fire();
}

void AWeaponBase::ClientFireEffect_Implementation(const FHitResult& HitInfo)
{	
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponData.ImpactFlash, FTransform(HitInfo.Location));
}

