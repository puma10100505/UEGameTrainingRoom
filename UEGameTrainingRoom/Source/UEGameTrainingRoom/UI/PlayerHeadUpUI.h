// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/UserWidgetBase.h"
#include "PlayerHeadUpUI.generated.h"


/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class UEGAMETRAININGROOM_API UPlayerHeadUpUI : public UUserWidgetBase
{
	GENERATED_BODY()
	
public:
	UPlayerHeadUpUI(const FObjectInitializer& ObjectInitializer);

	// UFUNCTION()
	// void OnHealthChanged(float NewHealth, float MaxHealth);
	//
	// UFUNCTION()
	// void OnArmorChanged(float NewArmor, float MaxArmor);

	virtual bool Initialize() override;

	UFUNCTION(BlueprintCallable)
	void InitializeModelDelegates(class ACharacterBase* OwnerCharacter);

	FORCEINLINE void SetHealthPercentage(float Health, float MaxHealth)
	{
		HealthPercent = CalcPercent(Health, MaxHealth);
	}
	
	FORCEINLINE void SetArmorPercentage(float Armor, float MaxArmor)
	{
		ArmorPercent = CalcPercent(Armor, MaxArmor);
	}

	FORCEINLINE void SetOwningCharacter(class ACharacterBase* InCharacter)
	{
		OwningCharacter = InCharacter;
	}

protected:
	UPROPERTY(BlueprintReadWrite, Category = Default)
	float HealthPercent;

	UPROPERTY(BlueprintReadWrite, Category = Default)
	float ArmorPercent;

	UPROPERTY(BlueprintReadOnly)
	class ACharacterBase* OwningCharacter;
};
