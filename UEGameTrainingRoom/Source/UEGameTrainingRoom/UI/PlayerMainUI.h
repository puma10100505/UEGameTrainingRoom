// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/UserWidgetBase.h"
#include "PlayerMainUI.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class UEGAMETRAININGROOM_API UPlayerMainUI : public UUserWidgetBase
{
	GENERATED_BODY()
	
public:
	virtual bool Initialize() override;

	FORCEINLINE void SetCurrentAmmoInClip(const int32 AmmoNum) 
	{
		CurrentAmmoInClip = AmmoNum;
	}

	FORCEINLINE void SetTotalCarriedAmmo(const int32 AmmoNum)
	{
		TotalCarriedAmmo = AmmoNum;
	}

	FORCEINLINE void SetHealth(const float CurrentHealth, const float MaxHealth) 
	{
		HealthPercent = CalcPercent(CurrentHealth, MaxHealth);
	}

	FORCEINLINE void SetArmor(const float CurrentArmor, const float MaxArmor)
	{
		ArmorPercent = CalcPercent(CurrentArmor, MaxArmor);
	}

protected:
	UPROPERTY(BlueprintReadWrite, Category = Default)
	float HealthPercent;

	UPROPERTY(BlueprintReadWrite, Category = Default)
	float ArmorPercent;

	UPROPERTY(BlueprintReadWrite, Category = Default)
	int32 CurrentAmmoInClip;

	UPROPERTY(BlueprintReadWrite, Category = Default)
	int32 TotalCarriedAmmo;
};
