// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Perception/AIPerceptionComponent.h"
#include "AIPerceptionBase.generated.h"

// DECLARE_LOG_CATEGORY_EXTERN(LogAIPerception, Log, All);

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup=AI, HideCategories=(Activation, Collision), meta=(BlueprintSpawnableComponent))
class UEGAMETRAININGROOM_API UAIPerceptionBase : public UAIPerceptionComponent
{
	GENERATED_BODY()
	
public:
	virtual void OnRegister() override;

protected:
	void TargetPerceptionUpdated(class AActor* Actor, FAIStimulus Stimulus);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = BBFields)
	FName BBField_TargetCharacter;
};
