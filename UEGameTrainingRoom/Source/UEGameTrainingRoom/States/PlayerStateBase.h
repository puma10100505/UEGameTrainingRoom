// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "Attributes/AttributeSetHealth.h"
#include "Attributes/AttributeSetWeapon.h"
#include "GameFramework/PlayerState.h"
#include "PlayerStateBase.generated.h"

/**
 * 
 */
UCLASS()
class UEGAMETRAININGROOM_API APlayerStateBase : public APlayerState, 
	public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	APlayerStateBase();

	virtual void BeginPlay() override;

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const;

	class UAttributeSetHealth* GetAttributeSetHealth() const ;

	class UAttributeSetWeapon* GetAttributeSetWeapon() const ;

	FORCEINLINE float GetHealth() const 
	{
		UAttributeSetHealth* AttrHealth = GetAttributeSetHealth();
		if (AttrHealth)
		{
			return AttrHealth->GetHealth();
		}
		else
		{
			return 0.f;
		}
	}

	FORCEINLINE float GetMaxHealth() const 
	{
		UAttributeSetHealth* AttrHealth = GetAttributeSetHealth();
		if (AttrHealth)
		{
			return AttrHealth->GetMaxHealth();
		}
		else
		{
			return 0.f;
		}
	}

	FORCEINLINE float GetArmor() const
	{
		UAttributeSetHealth* AttrHealth = GetAttributeSetHealth();
		if (AttrHealth)
		{
			return AttrHealth->GetArmor();
		}
		else
		{
			return 0.f;
		}
	}

	FORCEINLINE float GetMaxArmor() const
	{
		UAttributeSetHealth* AttrHealth = GetAttributeSetHealth();
		if (AttrHealth)
		{
			return AttrHealth->GetMaxArmor();
		}
		else
		{
			return 0.f;
		}
	}

	FORCEINLINE int32 GetCurrentAmmoInClip() const 
	{
		UAttributeSetWeapon* AttrWeapon = GetAttributeSetWeapon();
		if (AttrWeapon)
		{
			return (AttrWeapon->GetCurrentAmmoInClip() * 100 / 100);
		}
		else
		{
			return 0;
		}
	}

	FORCEINLINE int32 GetTotalCarriedAmmo() const 
	{
		UAttributeSetWeapon* AttrWeapon = GetAttributeSetWeapon();
		if (AttrWeapon)
		{
			return (AttrWeapon->GetTotalCarriedAmmo() * 100 / 100);
		}
		else
		{
			return 0;
		}
	}

protected:
	void OnHealthAttributeChanged(const FOnAttributeChangeData& Data);
	void OnArmorAttributeChanged(const FOnAttributeChangeData& Data);
	void OnCurrentAmmoInClipChanged(const FOnAttributeChangeData& Data);
	void OnTotalCarriedAmmoChanged(const FOnAttributeChangeData& Data);
	
protected:
	UPROPERTY()
	class UAbilitySystemComponent* AbilitySystem;

	UPROPERTY()
	class UAttributeSetHealth* AttributeSetHealth;

	UPROPERTY()
	class UAttributeSetWeapon* AttributeSetWeapon;

private:	// ASC Attribute Change Handles
	FDelegateHandle HealthAttributeChangeHandle;
	FDelegateHandle ArmorAttributeChangeHandle;
	FDelegateHandle CurrentAmmoInClipAttributeChangeHandle;
	FDelegateHandle TotalCarriedAmmoAttributeChangeHandle;
};
