// Fill out your copyright notice in the Description page of Project Settings.


#include "AIPerceptionBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CharacterBase.h"
#include "PlayerControllerBase.h"
#include "AIControllerBase.h"

// DEFINE_LOG_CATEGORY(LogAIPerception);

void UAIPerceptionBase::OnRegister()
{
    Super::OnRegister();
}

void UAIPerceptionBase::BeginPlay()
{
    Super::BeginPlay();

    OnTargetPerceptionUpdated.AddDynamic(this, &UAIPerceptionBase::TargetPerceptionUpdated);
}

void UAIPerceptionBase::TargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (Stimulus.WasSuccessfullySensed())
    {
        ACharacterBase* Character = Cast<ACharacterBase>(Actor);
        if (IsValid(Character) == false)
        {
            UE_LOG(LogTemp, Warning, TEXT("This is not a characterbase"));
            return;
        }

        AAIControllerBase* AIController = Cast<AAIControllerBase>(GetOwner());
        if (IsValid(AIController) == false)
        {
            UE_LOG(LogTemp, Warning, TEXT("Current owner player is not AIController"));
            return;
        }

        UBlackboardComponent* BBComp = AIController->GetBlackboardComponent();
        if (BBComp == nullptr)
        {
            UE_LOG(LogTemp, Warning, TEXT("Not found blackboard component"));
            return;
        }

        BBComp->SetValueAsObject(BBField_TargetCharacter, Character);

        UE_LOG(LogTemp, Log, TEXT("After set target character for BB, key: %s"), *BBField_TargetCharacter.ToString());
    }
}