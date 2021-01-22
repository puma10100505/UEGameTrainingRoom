// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility_CharacterReload.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "WeaponBase.h"
#include "CharacterBase.h"

UGameplayAbility_CharacterReload::UGameplayAbility_CharacterReload()
{

}

void UGameplayAbility_CharacterReload::ActivateAbility(const FGameplayAbilitySpecHandle Handle, 
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, 
    const FGameplayEventData* TriggerEventData)
{
    if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
    {
        if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
        {
            EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
            return;
        }

        Character = CastChecked<ACharacterBase>(ActorInfo->AvatarActor.Get());

        float ReloadTime = 0.f;
        if (IsValid(Character) && IsValid(Character->GetCurrentWeapon()))
        {
            ReloadTime = Character->GetCurrentWeapon()->GetWeaponModelData().ReloadTime;
        }

        UAbilityTask_WaitDelay* Task = UAbilityTask_WaitDelay::WaitDelay(this, ReloadTime);
        if (Task)
        {
            Task->OnFinish.AddDynamic(this, &UGameplayAbility_CharacterReload::OnReloadFinished);
            Task->ReadyForActivation();
        }
    }
}

void UGameplayAbility_CharacterReload::OnReloadFinished()
{
    if (IsValid(Character) && IsValid(Character->GetCurrentWeapon()))
    {
        Character->GetCurrentWeapon()->Reload();
    }

    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

bool UGameplayAbility_CharacterReload::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, 
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, 
    const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
    if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
    {
        return false;
    }

    const ACharacterBase* CurrentCharacter = CastChecked<ACharacterBase>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);
    bool ret = (CurrentCharacter && CurrentCharacter->CanReload());
    return ret;
}

void UGameplayAbility_CharacterReload::CancelAbility(const FGameplayAbilitySpecHandle Handle, 
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, 
    bool bReplicateCancelAbility)
{
    if (ScopeLockCount > 0)
    {
        WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UGameplayAbility_CharacterReload::CancelAbility, Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility));
        return ;
    }

    Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

    // ACharacterBase* CurrentCharacter = CastChecked<ACharacterBase>(ActorInfo->AvatarActor.Get());
    // if (CurrentCharacter)
    // {
    //     // CurrentCharacter->
    // }
}