// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility_CharacterFire.h"
#include "CharacterBase.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_Repeat.h"
#include "WeaponBase.h"

UGameplayAbility_CharacterFire::UGameplayAbility_CharacterFire()
{

}

void UGameplayAbility_CharacterFire::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			return;
		}

		Character = CastChecked<ACharacterBase>(ActorInfo->AvatarActor.Get());

		float FireInterval = 0.2f;
		if (IsValid(Character))
		{
			 AWeaponBase* CurrentWeapon = Character->GetCurrentWeapon();
			 if (IsValid(CurrentWeapon))
			 {
				 FireInterval = CurrentWeapon->GetWeaponModelData().FireInterval;
			 }
		}

		// Repeat暂不支持无限触发，先使用一个较大值代替
		UAbilityTask_Repeat* Task = UAbilityTask_Repeat::RepeatAction(this, FireInterval, 999);
		if (Task)
		{
			// Task->OnFinished.AddDynamic(this, &UGameplayAbility_CharacterFire::OnFireTimerFinished);
			Task->OnPerformAction.AddDynamic(this, &UGameplayAbility_CharacterFire::OnPerformFire);
			Task->ReadyForActivation();
		}
	}
}

void UGameplayAbility_CharacterFire::OnPerformFire(int32 Count)
{
	if (IsValid(Character) && IsValid(Character->GetCurrentWeapon()))
	{
		// Character->OnStartFire();
		Character->GetCurrentWeapon()->FireProcess();
	}
}

bool UGameplayAbility_CharacterFire::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const ACharacterBase* CurrentCharacter = CastChecked<ACharacterBase>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);
	return (CurrentCharacter && CurrentCharacter->CanFire());
}

void UGameplayAbility_CharacterFire::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UGameplayAbility_CharacterFire::CancelAbility, Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility));
		return;
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	
	ACharacterBase * CurrentCharacter = CastChecked<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (CurrentCharacter)
	{
		CurrentCharacter->OnStopFire();
	}
}

void UGameplayAbility_CharacterFire::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}