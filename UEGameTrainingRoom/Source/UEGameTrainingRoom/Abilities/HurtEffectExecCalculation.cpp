// Fill out your copyright notice in the Description page of Project Settings.


#include "HurtEffectExecCalculation.h"
#include "CharacterBase.h"
#include "AttributeSetHealth.h"

struct TRDamageStatics
{
    DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
    DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
    DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);

    TRDamageStatics()
    {
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSetHealth, Armor, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSetHealth, Health, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSetHealth, Damage, Target, false);
    }
};

static const TRDamageStatics& DamageStatics()
{
    static TRDamageStatics DStatics;
    return DStatics;
}

UHurtEffectExecCalculation::UHurtEffectExecCalculation() {}

void UHurtEffectExecCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, 
    OUT FGameplayEffectCustomExecutionOutput & OutExecutionOutput) const
{
    // 获取效果作用双方的ASC
    UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

    // 获取效果作用双方的Avatar Actor
	AActor* SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->AvatarActor : nullptr;
	AActor* TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->AvatarActor : nullptr;

    // 获取效果实例
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

    // 获取效果双方的Tags
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

    // float Armor = 0.f;
    // ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, Armor);
    // Armor = FMath::Max<float>(Armor, 0.f);

    // float Health = 0.f;
    // ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().HealthDef, EvaluationParameters, Health);
    // Health = FMath::Max<float>(Health, 0.f);

    float BaseDamage = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Gameplay.Weapon.BaseDamage")), false, -1.f);
    BaseDamage = FMath::Max<float>(BaseDamage, 0.f);

    float FinalDamage = BaseDamage;

    // TODO: 伤害计算过程

    if (FinalDamage > 0.f)
    {
        OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().DamageProperty, EGameplayModOp::Additive, -FinalDamage));
    }

    // TODO: Broadcast Damage to target asc
}