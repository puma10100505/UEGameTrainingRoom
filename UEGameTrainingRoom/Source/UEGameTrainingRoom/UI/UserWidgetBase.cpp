// Fill out your copyright notice in the Description page of Project Settings.


#include "UserWidgetBase.h"

DEFINE_LOG_CATEGORY(LogUI);

float UUserWidgetBase::CalcPercent(float NewVal, float MaxVal)
{
    if (MaxVal <= 0.0001f)
    {
        return 0.f;
    }

    NewVal = FMath::Max<float>(0.f, NewVal);

    return NewVal / MaxVal;
}