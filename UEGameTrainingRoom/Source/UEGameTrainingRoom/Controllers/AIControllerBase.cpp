// Fill out your copyright notice in the Description page of Project Settings.


#include "AIControllerBase.h"
#include "GameFramework/Pawn.h"
#include "AIPerceptionBase.h"

AAIControllerBase::AAIControllerBase(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionBase>(TEXT("AIPerception"));
}


void AAIControllerBase::BeginPlay()
{
    Super::BeginPlay();

    SetGenericTeamId(AITeamId);
}

ETeamAttitude::Type AAIControllerBase::GetTeamAttitudeTowards(const AActor& Other) const 
{
    if (const APawn* OtherPawn = Cast<APawn>(&Other))
    {
        if (const IGenericTeamAgentInterface* TeamAgent = 
            Cast<IGenericTeamAgentInterface>(OtherPawn->GetController()))
        {
            return Super::GetTeamAttitudeTowards(*OtherPawn->GetController());
        }
    }

    return ETeamAttitude::Neutral;
}
