// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStateBase.h"
#include "AbilitySystemComponent.h"
#include "Attributes/AttributeSetArmor.h"
#include "Attributes/AttributeSetHealth.h"


APlayerStateBase::APlayerStateBase()
{
    AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
    AbilitySystem->SetIsReplicated(true);
    AbilitySystem->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    AttributeSetHealth = CreateDefaultSubobject<UAttributeSetHealth>(TEXT("AttributeSetHealth"));
    AttributeSetArmor = CreateDefaultSubobject<UAttributeSetArmor>(TEXT("AttributeSetArmor"));

    NetUpdateFrequency = 100.f;
}

UAbilitySystemComponent* APlayerStateBase::GetAbilitySystemComponent() const
{
    return AbilitySystem;
}

UAttributeSetHealth* APlayerStateBase::GetAttributeSetHealth() const 
{
    return AttributeSetHealth;
}

UAttributeSetArmor* APlayerStateBase::GetAttributeSetArmor() const 
{
    return AttributeSetArmor;
}