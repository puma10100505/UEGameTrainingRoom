// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHeadUpUI.h"
#include "CharacterBase.h"


UPlayerHeadUpUI::UPlayerHeadUpUI(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    
}


bool UPlayerHeadUpUI::Initialize()
{
    return Super::Initialize();
}

void UPlayerHeadUpUI::InitializeModelDelegates(ACharacterBase* OwnerCharacter)
{
    if (IsValid(OwnerCharacter) == false)
    {
        UE_LOG(LogUI, Warning, TEXT("OwnerCharacter is null"));
        return;
    }
}