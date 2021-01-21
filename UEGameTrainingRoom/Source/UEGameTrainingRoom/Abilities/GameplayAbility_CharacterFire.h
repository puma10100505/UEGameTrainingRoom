// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "UObject/ObjectMacros.h"
#include "GameplayAbilitySpec.h"

#include "GameplayAbility_CharacterFire.generated.h"

/**
 * 
 */
UCLASS()
class UEGAMETRAININGROOM_API UGameplayAbility_CharacterFire : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGameplayAbility_CharacterFire();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override; 

	void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	// UFUNCTION()
	// void OnFireTimerFinished();

	UFUNCTION()
	void OnPerformFire(int32 Count);

private:
	UPROPERTY()
	class ACharacterBase* Character;
};
