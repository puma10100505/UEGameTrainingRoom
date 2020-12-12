// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMainUI.h"
#include "CharacterBase.h"
#include "WeaponBase.h"

bool UPlayerMainUI::Initialize()
{
    Super::Initialize();

    ACharacterBase* Character = GetOwningPlayerPawn<ACharacterBase>();
    if (IsValid(Character) == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("Not found owning character of this UI"));
        return false;
    }

    AWeaponBase* Weapon = Character->GetCurrentWeapon();
    if (IsValid(Weapon) == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("Not found current weapon"));
        return false;
    }
      
    Weapon->GetWeaponAmmoChangedDelegate().AddDynamic(this, &UPlayerMainUI::OnAmmoChanged);
    
    Character->GetHealthChangedDelegator().AddDynamic(this, &UPlayerMainUI::OnHealthChanged);
    Character->GetArmorChangedDelegator().AddDynamic(this, &UPlayerMainUI::OnArmorChanged);

    return true;
}

void UPlayerMainUI::OnAmmoChanged(int32 InCurrentAmmo, int32 InTotalLeftAmmo)
{
    CurrentAmmo = InCurrentAmmo;
    TotalLeftAmmo = InTotalLeftAmmo;    
}

void UPlayerMainUI::OnHealthChanged(float NewHealth, float MaxHealth)
{
    HealthPercent = CalcPercent(NewHealth, MaxHealth);
}

void UPlayerMainUI::OnArmorChanged(float NewArmor, float MaxArmor)
{
    ArmorPercent = CalcPercent(NewArmor, MaxArmor);
}