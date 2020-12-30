// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSetWeapon.h"
#include "Net/UnrealNetwork.h"

void UAttributeSetWeapon::OnRep_CurrentAmmoInClip(const FGameplayAttributeData& OldAmmoInClip)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetWeapon, CurrentAmmoInClip, OldAmmoInClip);
}

void UAttributeSetWeapon::OnRep_TotalCarriedAmmo(const FGameplayAttributeData& OldCarriedAmmo)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetWeapon, TotalCarriedAmmo, OldCarriedAmmo);
}

void UAttributeSetWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetWeapon, CurrentAmmoInClip, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetWeapon, TotalCarriedAmmo, COND_None, REPNOTIFY_Always);
}

void UAttributeSetWeapon::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);
    
}