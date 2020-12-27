// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/PlayerState.h"
#include "PlayerStateBase.generated.h"

/**
 * 
 */
UCLASS()
class UEGAMETRAININGROOM_API APlayerStateBase : public APlayerState, 
	public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	APlayerStateBase();

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const;

	class UAttributeSetHealth* GetAttributeSetHealth() const ;

	class UAttributeSetArmor* GetAttributeSetArmor() const ;

protected:
	class UAbilitySystemComponent* AbilitySystem;

	class UAttributeSetHealth* AttributeSetHealth;

	class UAttributeSetArmor* AttributeSetArmor;

};
