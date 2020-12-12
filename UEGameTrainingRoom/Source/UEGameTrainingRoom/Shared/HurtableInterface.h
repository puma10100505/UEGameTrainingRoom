#pragma once

#include "HurtableInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UHurtable: public UInterface 
{
    GENERATED_BODY()
};

class IHurtable
{
    GENERATED_BODY()
    
public:
    virtual void TakeHurt(const class AWeaponBase* SourceWeapon, float HitDistance);
};
