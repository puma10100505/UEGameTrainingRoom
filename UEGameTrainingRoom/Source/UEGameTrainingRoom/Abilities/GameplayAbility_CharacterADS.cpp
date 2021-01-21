// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility_CharacterADS.h"
#include "CharacterBase.h"

UGameplayAbility_CharacterADS::UGameplayAbility_CharacterADS()
{

}

bool UGameplayAbility_CharacterADS::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, 
    OUT FGameplayTagContainer* OptionalRelevantTags) const
{
    if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
    {
        return false;
    }

    const ACharacterBase* CurrentCharacter = CastChecked<ACharacterBase>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);
    return (CurrentCharacter && CurrentCharacter->GetIsPreparedForBattle());
}

void UGameplayAbility_CharacterADS::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, 
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (HasAuthorityOrPredictionKey(OwnerInfo, &ActivationInfo))
    {
        if (!CommitAbility(Handle, OwnerInfo, ActivationInfo))
        {
            EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
            return;
        }

        ACharacterBase* CurrentCharacter = CastChecked<ACharacterBase>(OwnerInfo->AvatarActor.Get());
        if (IsValid(CurrentCharacter))
        {
            CurrentCharacter->SwitchToAiming();
        }
    }
}

void UGameplayAbility_CharacterADS::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
    if (ScopeLockCount > 0)
    {
        WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UGameplayAbility_CharacterADS::CancelAbility, Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility));
        return;
    }

    Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

    ACharacterBase* CurrentCharacter = CastChecked<ACharacterBase>(ActorInfo->AvatarActor.Get());

    if (CurrentCharacter)
    {
        CurrentCharacter->RecoveryFromAiming();
    }
}