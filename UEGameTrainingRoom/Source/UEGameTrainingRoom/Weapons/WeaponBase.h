// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponModelData.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "Attributes/AttributeSetWeapon.h"

#include "WeaponBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogWeapon, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponAmmoChanged, int32, CurrentAmmo, int32, TotalLeftAmmo);

UCLASS(Blueprintable)
class UEGAMETRAININGROOM_API AWeaponBase : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Config")
	FWeaponModelData WeaponData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Effects")
	TSubclassOf<class UGameplayEffect> HurtEffect;

public:	
	// Sets default values for this actor's properties
	AWeaponBase();

	FORCEINLINE const FWeaponModelData& GetWeaponModelData() const { return WeaponData; }
	// FORCEINLINE int32 GetCurrentAmmo() const { return CurrentAmmo; }
	// FORCEINLINE int32 GetTotalLeftAmmo() const { return TotalLeftAmmo; }
	// FORCEINLINE FWeaponAmmoChanged& GetWeaponAmmoChangedDelegate() 
	// {
	// 	return WeaponAmmoChangedDelegate; 
	// }

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	void FireProcess();
	bool CanFire();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	virtual bool WeaponLineTrace_Single(FHitResult& HitInfo);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void InitializeWeaponData();

	virtual void ConsumeAmmo();

	virtual bool NeedReload();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Fire();

	void AttachTo(class ACharacterBase* Pawn);

	class ACharacterBase* GetOwnerCharacter() const;

	class UAttributeSetWeapon* GetWeaponAttributeSet() const ;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentAmmoInClip() const 
	{
		UAttributeSetWeapon* Attr = GetWeaponAttributeSet();
		if (Attr)
		{
			return Attr->GetCurrentAmmoInClip();
		}
		else 
		{
			return 0.f;
		}
	}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetTotalCarriedAmmo() const 
	{
		UAttributeSetWeapon* Attr = GetWeaponAttributeSet();
		if (Attr)
		{
			return Attr->GetTotalCarriedAmmo();
		}
		else 
		{
			return 0.f;
		}
	}

	virtual bool CanReload() const;
	virtual void Reload();

protected:
	// [Server + Local]
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ServerFire();
	bool ServerFire_Validate();
	void ServerFire_Implementation();

	UFUNCTION(NetMulticast, Unreliable)
	virtual void ClientFireEffect(bool IsHit, const FHitResult& HitInfo);
	void ClientFireEffect_Implementation(bool IsHit, const FHitResult& HitInfo);

	UFUNCTION(NetMulticast, Unreliable)
	virtual void ClientFireMuzzleEffect(const FName& MuzzleName, class USoundBase* MuzzleSound, 
		class UParticleSystem* MuzzleFlash, class ACharacterBase* InCharacter);
	void ClientFireMuzzleEffect_Implementation(const FName& MuzzleName, class USoundBase* MuzzleSound, 
		class UParticleSystem* MuzzleFlash, class ACharacterBase* InCharacter);

	// UFUNCTION()
	// void OnRep_CurrentAmmo(int32 OldCurrentAmmo);

	// UFUNCTION()
	// void OnRep_TotalLeftAmmo(int32 OldTotalLeftAmmo);

protected:
	

	UPROPERTY(BlueprintReadWrite, Replicated, Category = Owner)
	class ACharacterBase* OwnerCharacter;

	// TODO: AttributeSetWeapon
	// UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_CurrentAmmo, Category = WeaponData)
	// int32 CurrentAmmo;

	// // TODO: AttributeSetWeapon
	// UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_TotalLeftAmmo, Category = WeaponData)
	// int32 TotalLeftAmmo;

	// UPROPERTY(BlueprintAssignable, Category = EventDispachers)
	// FWeaponAmmoChanged WeaponAmmoChangedDelegate;


};
