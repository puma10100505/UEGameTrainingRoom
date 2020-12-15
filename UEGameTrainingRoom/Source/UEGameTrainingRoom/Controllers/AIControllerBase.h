// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GenericTeamAgentInterface.h"

#include "AIControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class UEGAMETRAININGROOM_API AAIControllerBase : public AAIController
{
	GENERATED_BODY()

public:
	AAIControllerBase(const FObjectInitializer& ObjectInitializer);

	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGenericTeamId AITeamId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Components)
	class UAIPerceptionBase* AIPerceptionComp;
};
