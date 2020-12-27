// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AttributeDefine.h"
#include "AbilitySystemComponent.h"


#include "AttributeSetHealth.generated.h"

/**
 * 
 */
UCLASS()
class UEGAMETRAININGROOM_API UAttributeSetHealth : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, ReplicatedUsing = OnRep_Health, Category="Character|AttributeSetHealth")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAttributeSetHealth, Health)

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, ReplicatedUsing = OnRep_MaxHealth, Category="Character|AttributeSetHealth")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAttributeSetHealth, MaxHealth)
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& Data);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& Data);
};
