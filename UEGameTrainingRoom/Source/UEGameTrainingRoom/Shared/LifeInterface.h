#pragma once

#include "CoreMinimal.h"
#include "LifeInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class ULife: public UInterface 
{
    GENERATED_BODY()
};

class ILife
{
    GENERATED_BODY()

public:
    UFUNCTION()
    virtual void ChangeLife(float Value);
};