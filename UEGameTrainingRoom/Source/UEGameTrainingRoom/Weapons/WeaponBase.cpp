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
#include "Components/ArrowComponent.h"
#include "States/PlayerStateBase.h"


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


	//  WeaponAmmoChangedDelegate.Broadcast(CurrentAmmo, TotalLeftAmmo);
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponBase::ClientFireMuzzleEffect_Implementation(const FName& MuzzleName, USoundBase* MuzzleSound, UParticleSystem* MuzzleFlash, 
	class ACharacterBase* InCharacter)
{
	// Play Fire Sound2D
	UGameplayStatics::PlaySound2D(GetWorld(), MuzzleSound);

	if (IsValid(InCharacter))
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleFlash,
			InCharacter->GetMesh(), MuzzleName,
			InCharacter->GetMesh()->GetSocketLocation(MuzzleName),
			InCharacter->GetMesh()->GetSocketRotation(MuzzleName),
			//InCharacter->GetArrowComponent()->GetForwardVector().Rotation(),
			FVector(1.0f, 1.0f, 1.0f),
			EAttachLocation::KeepWorldPosition);
		

		UE_LOG(LogWeapon, Log, TEXT("After emitter attach"));
	}
}

void AWeaponBase::Fire()
{
	if (CanFire())
	{
		UE_LOG(LogWeapon, Log, TEXT("Entry of fire, LocalRole: %d, NetMode: %d"), GetLocalRole(), GetNetMode());
		if (GetLocalRole() < ROLE_Authority)
		{
			UE_LOG(LogWeapon, Log, TEXT("Before invoke fire server rpc"));

			// Client
			ServerFire();
		}

		FireProcess();
	}
}

UAbilitySystemComponent* AWeaponBase::GetAbilitySystemComponent() const
{
	if (IsValid(GetOwnerCharacter()))
	{
		return GetOwnerCharacter()->GetAbilitySystemComponent();
	}
	else
	{
		return nullptr;
	}
}

void AWeaponBase::FireProcess()
{
	UE_LOG(LogWeapon, Log, TEXT("Entry of fire process"));

	ConsumeAmmo();

	// 射线命中检测
	FHitResult HitInfo;
	bool IsHit = WeaponLineTrace_Single(HitInfo);

	// Take Damage
	if (IsHit /* && GetOwnerCharacter() && GetOwnerCharacter()->GetLocalRole() == ROLE_Authority*/)
	{
		if (ACharacterBase* HitCharacter = Cast<ACharacterBase>(HitInfo.GetActor()))
		{
			if (IsValid(HitCharacter->GetAbilitySystemComponent()) && IsValid(GetAbilitySystemComponent()))
			{
				if (HurtEffect)
				{
					FGameplayEffectContextHandle EffectContext = GetAbilitySystemComponent()->MakeEffectContext();
					EffectContext.AddSourceObject(this);
					FGameplayEffectSpecHandle NewHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(HurtEffect, 1.f, EffectContext);
					if (NewHandle.IsValid())
					{
						NewHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Gameplay.Weapon.BaseDamage")), GetWeaponModelData().Damage);
						GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), HitCharacter->GetAbilitySystemComponent());
					}
				}
			} 
			else
			{
				UE_LOG(LogCharacter, Error, TEXT("Not found ASC of target or self"));
			}
		}
	}

	// Fire client VFX
	if (GetLocalRole() == ROLE_Authority)
	{
		ClientFireEffect(IsHit, HitInfo);

		if (IsValid(GetOwnerCharacter()))
		{
			ClientFireMuzzleEffect(WeaponData.MuzzleName, WeaponData.MuzzleSound, 
				WeaponData.MuzzleFlash, GetOwnerCharacter());
		}
	}
}

bool AWeaponBase::CanFire()
{
	UAttributeSetWeapon* Attr = GetWeaponAttributeSet();
	if (Attr == nullptr)
	{
		return false;
	}

	if (Attr->GetCurrentAmmoInClip() <= 0) 
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

	TArray<AActor*> IgnoreActors;
	if (IsValid(GetOwnerCharacter()))
	{
		IgnoreActors.Add(GetOwnerCharacter());
	}

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActors(IgnoreActors);
	QueryParams.bReturnPhysicalMaterial = true;
	bool bIsHit = GetWorld()->LineTraceSingleByChannel(HitInfo, StartPos, EndPos, ECC_Pawn, QueryParams);

	return bIsHit;
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponBase, OwnerCharacter);
	// DOREPLIFETIME(AWeaponBase, CurrentAmmo);
	// DOREPLIFETIME(AWeaponBase, TotalLeftAmmo);
}

void AWeaponBase::InitializeWeaponData()
{
	// CurrentAmmo = WeaponData.ClipCapacity;
	// TotalLeftAmmo = WeaponData.MaxAmmo;

	UAttributeSetWeapon* AttributeSetWeapon = GetWeaponAttributeSet();
	if (AttributeSetWeapon)
	{
		AttributeSetWeapon->SetCurrentAmmoInClip(WeaponData.ClipCapacity);
		AttributeSetWeapon->SetTotalCarriedAmmo(WeaponData.MaxAmmo);
	}
}

void AWeaponBase::ConsumeAmmo()
{
	UAttributeSetWeapon* AttributeSetWeapon = GetWeaponAttributeSet();
	if (AttributeSetWeapon == nullptr)
	{
		UE_LOG(LogWeapon, Error, TEXT("Not found AttributeSetWeapon"));
		return ;
	}

	AttributeSetWeapon->SetCurrentAmmoInClip(
		AttributeSetWeapon->GetCurrentAmmoInClip() - WeaponData.BulletNumPerShoot);

	if (NeedReload())
	{
		if (GetOwnerCharacter())
		{
			GetOwnerCharacter()->ActivateReloadAbility();
		}
	}
}

bool AWeaponBase::NeedReload()
{
	UAttributeSetWeapon* AttributeSetWeapon = GetWeaponAttributeSet();

	if (AttributeSetWeapon == nullptr)
	{
		return false;
	}

	return AttributeSetWeapon->GetCurrentAmmoInClip() <= 0;
}

void AWeaponBase::Reload()
{
	UAttributeSetWeapon* AttributeSetWeapon = GetWeaponAttributeSet();
	if (!AttributeSetWeapon)
	{
		UE_LOG(LogWeapon, Error, TEXT("Not found weapon attribute"));
		return;
	}

	// 换弹数值变更
	int32 AmmoDiff = WeaponData.ClipCapacity - AttributeSetWeapon->GetCurrentAmmoInClip();
	if (AmmoDiff <= 0)
	{
		return;
	}

	if (AmmoDiff >= AttributeSetWeapon->GetTotalCarriedAmmo())
	{
		AttributeSetWeapon->SetCurrentAmmoInClip(
			AttributeSetWeapon->GetCurrentAmmoInClip() + AttributeSetWeapon->GetTotalCarriedAmmo());
		AttributeSetWeapon->SetTotalCarriedAmmo(0);
	}
	else
	{
		AttributeSetWeapon->SetCurrentAmmoInClip(WeaponData.ClipCapacity);
		AttributeSetWeapon->SetTotalCarriedAmmo(AttributeSetWeapon->GetTotalCarriedAmmo() - AmmoDiff);
	}
}

bool AWeaponBase::CanReload() const
{
	UAttributeSetWeapon* AttributeSetWeapon = GetWeaponAttributeSet();
	if (AttributeSetWeapon == nullptr)
	{
		return false;
	}

	return AttributeSetWeapon->GetTotalCarriedAmmo() > 0;
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

void AWeaponBase::ClientFireEffect_Implementation(bool IsHit, const FHitResult& HitInfo)
{	
	if (IsHit)
	{
		// Impact effect
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponData.ImpactFlash, FTransform(HitInfo.Location));
		// Impact sound
		UGameplayStatics::PlaySound2D(GetWorld(), WeaponData.ImpactSound);

		// Hit React
		if (ACharacterBase* HitCharacter = Cast<ACharacterBase>(HitInfo.Actor))
		{
			if (HitCharacter->GetHitReactAnim())
			{
				// TODO: 这里应该不能直接播放动画，而应该通过数据通知人物动画蓝图进行动作混合
				HitCharacter->GetMesh()->PlayAnimation(HitCharacter->GetHitReactAnim(), false);
			}
		}
	}
	
	if (IsValid(GetOwnerCharacter()))
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FVector TrailStart_Loc = GetOwnerCharacter()->GetMesh()->GetSocketLocation(WeaponData.MuzzleName);
		FVector TrailStop_Loc = HitInfo.Location;
		if (IsHit == false)
		{
			TrailStop_Loc = HitInfo.TraceEnd;
		}
		
		FRotator TrailStart_Rot = (TrailStop_Loc - TrailStart_Loc).Rotation();

		GetWorld()->SpawnActor<AActor>(WeaponData.GunTrail, TrailStart_Loc, TrailStart_Rot, Params);
	} 
	else 
	{
		UE_LOG(LogWeapon, Log, TEXT("Owner character is not exist in client"));
	}
}

// void AWeaponBase::OnRep_CurrentAmmo(int32 OldCurrentAmmo)
// {
// 	if (OldCurrentAmmo != CurrentAmmo)
// 	{
// 		WeaponAmmoChangedDelegate.Broadcast(CurrentAmmo, TotalLeftAmmo);
// 	}
// }

// void AWeaponBase::OnRep_TotalLeftAmmo(int32 OldTotalLeftAmmo)
// {
// 	if (OldTotalLeftAmmo != TotalLeftAmmo)
// 	{
// 		WeaponAmmoChangedDelegate.Broadcast(CurrentAmmo, TotalLeftAmmo);
// 	}
// }

UAttributeSetWeapon* AWeaponBase::GetWeaponAttributeSet() const
{
	if (IsValid(OwnerCharacter))
	{
		APlayerStateBase* PS = OwnerCharacter->GetPlayerState<APlayerStateBase>();
		if (PS)
		{
			return PS->GetAttributeSetWeapon();
		}
	}

	return nullptr;
}