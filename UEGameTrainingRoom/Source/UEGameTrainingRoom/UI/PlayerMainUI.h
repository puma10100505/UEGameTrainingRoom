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

protected:
	UFUNCTION()
	void OnAmmoChanged(int32 InCurrentAmmo, int32 InTotalLeftAmmo);

	UFUNCTION()
	void OnHealthChanged(float NewHealth, float MaxHealth);

	UFUNCTION()
	void OnArmorChanged(float NewArmor, float MaxArmor);

protected:
	UPROPERTY(BlueprintReadWrite, Category = Default)
	float HealthPercent;

	UPROPERTY(BlueprintReadWrite, Category = Default)
	float ArmorPercent;

	UPROPERTY(BlueprintReadWrite, Category = Default)
	int32 CurrentAmmo;

	UPROPERTY(BlueprintReadWrite, Category = Default)
	int32 TotalLeftAmmo;
};
