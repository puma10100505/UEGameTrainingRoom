// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSetArmor.h"
#include "Net/UnrealNetwork.h"

void UAttributeSetArmor::OnRep_Armor(const FGameplayAttributeData& OldArmor)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetArmor, Armor, OldArmor);
}

void UAttributeSetArmor::OnRep_MaxArmor(const FGameplayAttributeData& OldMaxArmor)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetArmor, MaxArmor, OldMaxArmor);
}

void UAttributeSetArmor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetArmor, Armor, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetArmor, MaxArmor, COND_None, REPNOTIFY_Always);
}
