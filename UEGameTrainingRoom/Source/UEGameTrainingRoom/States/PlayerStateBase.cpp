// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStateBase.h"
#include "Attributes/AttributeSetArmor.h"
#include "Attributes/AttributeSetHealth.h"


APlayerStateBase::APlayerStateBase()
{
    // TODO: 初始化属性集和ASC
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