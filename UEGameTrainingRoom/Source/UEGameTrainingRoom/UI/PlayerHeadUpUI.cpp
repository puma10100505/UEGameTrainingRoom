// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHeadUpUI.h"
#include "CharacterBase.h"


UPlayerHeadUpUI::UPlayerHeadUpUI(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    
}

void UPlayerHeadUpUI::OnHealthChanged(float NewHealth, float MaxHealth)
{
    HealthPercent = CalcPercent(NewHealth, MaxHealth);
}

void UPlayerHeadUpUI::OnArmorChanged(float NewArmor, float MaxArmor)
{
    ArmorPercent = CalcPercent(NewArmor, MaxArmor);
}

bool UPlayerHeadUpUI::Initialize()
{
    return Super::Initialize();

/*
    ACharacterBase* Character = GetOwningPlayerPawn<ACharacterBase>();
    if (IsValid(Character) == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("Not found owning character of this UI"));
        return false;
    }

    Character->GetHealthChangedDelegator().AddDynamic(this, &UPlayerHeadUpUI::OnHealthChanged);
    Character->GetArmorChangedDelegator().AddDynamic(this, &UPlayerHeadUpUI::OnArmorChanged);

    return true;
    */
}

void UPlayerHeadUpUI::InitializeModelDelegates(ACharacterBase* OwnerCharacter)
{
    if (IsValid(OwnerCharacter) == false)
    {
        UE_LOG(LogUI, Warning, TEXT("OwnerCharacter is null"));
        return;
    }

    if (OwnerCharacter->GetHealthChangedDelegator().IsBound() == false)
    {
        OwnerCharacter->GetHealthChangedDelegator().AddDynamic(this, &UPlayerHeadUpUI::OnHealthChanged);
    }

    if (OwnerCharacter->GetArmorChangedDelegator().IsBound() == false)
    {
        OwnerCharacter->GetArmorChangedDelegator().AddDynamic(this, &UPlayerHeadUpUI::OnArmorChanged);
    }
}