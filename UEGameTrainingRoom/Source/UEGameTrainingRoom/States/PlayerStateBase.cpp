// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStateBase.h"
#include "AbilitySystemComponent.h"
#include "CharacterBase.h"
#include "PlayerHeadUpUI.h"
#include "PlayerControllerBase.h"
#include "PlayerMainUI.h"


APlayerStateBase::APlayerStateBase()
{
    AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
    AbilitySystem->SetIsReplicated(true);
    AbilitySystem->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    AttributeSetHealth = CreateDefaultSubobject<UAttributeSetHealth>(TEXT("AttributeSetHealth"));
    AttributeSetWeapon = CreateDefaultSubobject<UAttributeSetWeapon>(TEXT("AttributeSetWeapon"));

    NetUpdateFrequency = 100.f;
}

void APlayerStateBase::BeginPlay() 
{
    Super::BeginPlay();

    HealthAttributeChangeHandle = AbilitySystem->GetGameplayAttributeValueChangeDelegate(AttributeSetHealth->GetHealthAttribute()).AddUObject(this, &APlayerStateBase::OnHealthAttributeChanged);
    ArmorAttributeChangeHandle = AbilitySystem->GetGameplayAttributeValueChangeDelegate(AttributeSetHealth->GetArmorAttribute()).AddUObject(this, &APlayerStateBase::OnArmorAttributeChanged);
    CurrentAmmoInClipAttributeChangeHandle = AbilitySystem->GetGameplayAttributeValueChangeDelegate(AttributeSetWeapon->GetCurrentAmmoInClipAttribute()).AddUObject(this, &APlayerStateBase::OnCurrentAmmoInClipChanged);
    TotalCarriedAmmoAttributeChangeHandle = AbilitySystem->GetGameplayAttributeValueChangeDelegate(AttributeSetWeapon->GetTotalCarriedAmmoAttribute()).AddUObject(this, &APlayerStateBase::OnTotalCarriedAmmoChanged);
}

void APlayerStateBase::OnCurrentAmmoInClipChanged(const FOnAttributeChangeData& Data)
{
    ACharacterBase* Character = Cast<ACharacterBase>(GetPawn());
    if (Character)
    {
        // 只有玩家自己可以看到MAINUI的变更
        if (Character->IsPlayerControlled() && Character->IsLocallyControlled())
        {
            APlayerControllerBase* PC = Cast<APlayerControllerBase>(Character->GetController());
            if (IsValid(PC) == false)
            {
                UE_LOG(LogCharacter, Warning, TEXT("Not found player controller"));
                return;
            }

            UPlayerMainUI* Widget = PC->GetMainUI();
            if (Widget)
            {
                Widget->SetCurrentAmmoInClip(Data.NewValue);
            }
        }
    }
}

void APlayerStateBase::OnTotalCarriedAmmoChanged(const FOnAttributeChangeData& Data)
{
    ACharacterBase* Character = Cast<ACharacterBase>(GetPawn());
    if (Character)
    {
        // 只有玩家自己可以看到MAINUI的变更
        if (Character->IsPlayerControlled() && Character->IsLocallyControlled())
        {
            APlayerControllerBase* PC = Cast<APlayerControllerBase>(Character->GetController());
            if (IsValid(PC) == false)
            {
                UE_LOG(LogCharacter, Warning, TEXT("Not found player controller"));
                return;
            }

            UPlayerMainUI* Widget = PC->GetMainUI();
            if (Widget)
            {
                Widget->SetTotalCarriedAmmo(Data.NewValue);
            }
        }
    }
}

void APlayerStateBase::OnHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
    ACharacterBase* Character = Cast<ACharacterBase>(GetPawn());
    if (Character)
    {
        if (Character->IsPlayerControlled() == false || Character->IsLocallyControlled() == false)
        {
            UPlayerHeadUpUI* HeadUpUI = Character->GetHeadUpUIInstance();
            if (HeadUpUI)
            {
                HeadUpUI->SetHealthPercentage(Data.NewValue, AttributeSetHealth->GetMaxHealth());
            }
        }
        else
        {
            APlayerControllerBase* PC = Cast<APlayerControllerBase>(Character->GetController());
            if (IsValid(PC) == false)
            {
                UE_LOG(LogCharacter, Warning, TEXT("Not found player controller"));
                return;
            }

            UPlayerMainUI* Widget = PC->GetMainUI();
            if (Widget)
            {
                Widget->SetHealth(Data.NewValue, GetMaxHealth());
            }
        }
    }
}

void APlayerStateBase::OnArmorAttributeChanged(const FOnAttributeChangeData& Data)
{
    ACharacterBase* Character = Cast<ACharacterBase>(GetPawn());
    if (Character)
    {
        if (Character->IsPlayerControlled() == false || Character->IsLocallyControlled() == false)
        {
            UPlayerHeadUpUI* HeadUpUI = Character->GetHeadUpUIInstance();
            if (HeadUpUI)
            {
                HeadUpUI->SetArmorPercentage(Data.NewValue, AttributeSetHealth->GetMaxArmor());
            }
        }
        else
        {
            APlayerControllerBase* PC = Cast<APlayerControllerBase>(Character->GetController());
            if (IsValid(PC) == false)
            {
                UE_LOG(LogCharacter, Warning, TEXT("Not found player controller"));
                return;
            }

            UPlayerMainUI* Widget = PC->GetMainUI();
            if (Widget)
            {
                Widget->SetArmor(Data.NewValue, GetMaxArmor());
            }
        }
    }
}


UAbilitySystemComponent* APlayerStateBase::GetAbilitySystemComponent() const
{
    return AbilitySystem;
}

UAttributeSetHealth* APlayerStateBase::GetAttributeSetHealth() const 
{
    return AttributeSetHealth;
}

// UAttributeSetArmor* APlayerStateBase::GetAttributeSetArmor() const 
// {
//     return AttributeSetArmor;
// }

UAttributeSetWeapon* APlayerStateBase::GetAttributeSetWeapon() const 
{
    return AttributeSetWeapon;
}