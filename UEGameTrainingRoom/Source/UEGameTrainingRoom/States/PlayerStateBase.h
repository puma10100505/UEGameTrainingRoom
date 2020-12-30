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

	virtual void BeginPlay() override;

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const;

	class UAttributeSetHealth* GetAttributeSetHealth() const ;

	class UAttributeSetArmor* GetAttributeSetArmor() const ;

	class UAttributeSetWeapon* GetAttributeSetWeapon() const ;

protected:
	UPROPERTY()
	class UAbilitySystemComponent* AbilitySystem;

	UPROPERTY()
	class UAttributeSetHealth* AttributeSetHealth;

	UPROPERTY()
	class UAttributeSetArmor* AttributeSetArmor;

	UPROPERTY()
	class UAttributeSetWeapon* AttributeSetWeapon;
};
