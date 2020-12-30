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

		// if (MainUIClass != nullptr)
		// {
		// 	MainUI = CreateWidget<UUserWidgetBase>(this, MainUIClass);
		// 	if (IsValid(MainUI))
		// 	{
		// 		MainUI->AddToViewport();
		// 	}
		// }
	}
}

void APlayerControllerBase::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
}

void APlayerControllerBase::CreateMainUI()
{
	// TODO: 创建玩家主UI界面
}

void APlayerControllerBase::CreateHeadUpUI()
{
	// TODO: 创建人物头顶UI信息条，只对当前控制玩家以外的人物对象创建
}