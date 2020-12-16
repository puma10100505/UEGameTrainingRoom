// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControllerBase.h"
#include "Blueprint/UserWidget.h"
#include "UserWidgetBase.h"



void APlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();

	
	if (GetNetMode() == ENetMode::NM_Client)
	{
		if (CrosshairClass != nullptr)
		{
			CrosshairUI = CreateWidget<UUserWidgetBase>(this, CrosshairClass);
			if (IsValid(CrosshairUI))
			{
				CrosshairUI->AddToViewport();
			}
		}

		if (MainUIClass != nullptr)
		{
			MainUI = CreateWidget<UUserWidgetBase>(this, MainUIClass);
			if (IsValid(MainUI))
			{
				MainUI->AddToViewport();
			}
		}
	}
}

void APlayerControllerBase::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
}
