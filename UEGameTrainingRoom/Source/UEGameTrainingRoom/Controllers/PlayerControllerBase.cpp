// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControllerBase.h"
#include "Blueprint/UserWidget.h"
#include "UserWidgetBase.h"

void APlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();

	/*
	if (IsLocalController())
	{
		if (CrosshairClass != nullptr)
		{
			CrosshairUI = CreateWidget<UUserWidgetBase>(this, CrosshairClass);
			if (IsValid(CrosshairUI))
			{
				CrosshairUI->AddToViewport();
			}
		}
	}
	*/
}

void APlayerControllerBase::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
}
