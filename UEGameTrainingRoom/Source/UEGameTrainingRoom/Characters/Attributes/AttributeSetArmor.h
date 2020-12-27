// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AttributeDefine.h"


#include "AttributeSetArmor.generated.h"

/**
 * 
 */
UCLASS()
class UEGAMETRAININGROOM_API UAttributeSetArmor : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, ReplicatedUsing = OnRep_Armor, Category = "Character|AttributeSetArmor")
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UAttributeSetArmor, Armor)

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, ReplicatedUsing = OnRep_Armor, Category = "Character|AttributeSetArmor")
	FGameplayAttributeData MaxArmor;
	ATTRIBUTE_ACCESSORS(UAttributeSetArmor, MaxArmor)

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor);

	UFUNCTION()
	void OnRep_MaxArmor(const FGameplayAttributeData& OldMaxArmor);

};
