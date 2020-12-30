// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStateBase.h"
#include "AbilitySystemComponent.h"
#include "Attributes/AttributeSetArmor.h"
#include "Attributes/AttributeSetHealth.h"
#include "Attributes/AttributeSetWeapon.h"


APlayerStateBase::APlayerStateBase()
{
    AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
    AbilitySystem->SetIsReplicated(true);
    AbilitySystem->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    AttributeSetHealth = CreateDefaultSubobject<UAttributeSetHealth>(TEXT("AttributeSetHealth"));
    AttributeSetArmor = CreateDefaultSubobject<UAttributeSetArmor>(TEXT("AttributeSetArmor"));
    AttributeSetWeapon = CreateDefaultSubobject<UAttributeSetWeapon>(TEXT("AttributeSetWeapon"));

    NetUpdateFrequency = 100.f;
}

void APlayerStateBase::BeginPlay() 
{
    Super::BeginPlay();

    // TODO: 注册属性变更回调, 在属性变更处理函数中调用UI接口更新UI数据
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

UAttributeSetWeapon* APlayerStateBase::GetAttributeSetWeapon() const 
{
    return AttributeSetWeapon;
}