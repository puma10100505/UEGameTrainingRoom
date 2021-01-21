// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSetHealth.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "CharacterBase.h"
#include "Net/UnrealNetwork.h"

void UAttributeSetHealth::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetHealth, Health, OldHealth);
}

void UAttributeSetHealth::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetHealth, MaxHealth, OldMaxHealth);
}

void UAttributeSetHealth::OnRep_Armor(const FGameplayAttributeData& OldArmor)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetHealth, Armor, OldArmor);
}

void UAttributeSetHealth::OnRep_MaxArmor(const FGameplayAttributeData& OldMaxArmor)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetHealth, MaxArmor, OldMaxArmor);
}

void UAttributeSetHealth::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetHealth, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetHealth, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetHealth, Armor, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetHealth, MaxArmor, COND_None, REPNOTIFY_Always);
}

void UAttributeSetHealth::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);
    
    if (Data.EvaluatedData.Attribute == GetDamageAttribute())
    {
        if (Data.Target.AbilityActorInfo.IsValid() == false || Data.Target.AbilityActorInfo->AvatarActor.IsValid() == false)
        {
            UE_LOG(LogCharacter, Error, TEXT("ActorInfo is not valid"));
            return;
        }

        ACharacterBase* TargetCharacter = Cast<ACharacterBase>(Data.Target.AbilityActorInfo->AvatarActor.Get());
        if (IsValid(TargetCharacter) == false)
        {
            return;
        }

        const float RealDamage = GetDamage();
        SetDamage(0.f);
        if (RealDamage != 0.f)
        {
            if (TargetCharacter->IsAlive() == false)
            {
                return;
            }

            float CurrentArmor = GetArmor();
            float CurrentHealth = GetHealth();

            if (CurrentArmor > 0)
            {
                CurrentArmor += RealDamage;
                if (CurrentArmor < 0)
                {
                    CurrentHealth += CurrentArmor;
                    SetArmor(0);
                }
            }
            else
            {
                CurrentHealth += RealDamage;
            }

            if (CurrentHealth < 0)
            {
                CurrentHealth = 0;
            }

            SetHealth(CurrentHealth);
            SetArmor(CurrentArmor);
        }

        if (GetHealth() <= 0)
        {
            TargetCharacter->KillToDeath();
        }
    }
}

