// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserWidgetBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogUI, Log, All);

/**
 * 
 */
UCLASS(Blueprintable)
class UEGAMETRAININGROOM_API UUserWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UUserWidgetBase(const FObjectInitializer& ObjectInitializer)
		:Super(ObjectInitializer){}

protected:
	virtual float CalcPercent(float NewVal, float MaxVal);
};
