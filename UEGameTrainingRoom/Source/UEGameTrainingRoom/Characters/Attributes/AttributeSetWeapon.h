// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AttributeDefine.h"
#include "AbilitySystemComponent.h"


#include "AttributeSetWeapon.generated.h"

/**
 * 
 */
UCLASS()
class UEGAMETRAININGROOM_API UAttributeSetWeapon : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, ReplicatedUsing = OnRep_CurrentAmmoInClip, Category="Character|AttributeSetWeapon")
	FGameplayAttributeData CurrentAmmoInClip;
	ATTRIBUTE_ACCESSORS(UAttributeSetWeapon, CurrentAmmoInClip)

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, ReplicatedUsing = OnRep_TotalCarriedAmmo, Category="Character|AttributeSetWeapon")
	FGameplayAttributeData TotalCarriedAmmo;
	ATTRIBUTE_ACCESSORS(UAttributeSetWeapon, TotalCarriedAmmo)
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_CurrentAmmoInClip(const FGameplayAttributeData& Data);

	UFUNCTION()
	void OnRep_TotalCarriedAmmo(const FGameplayAttributeData& Data);

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};
