// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControllerBase.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStateBase.h"
#include "PlayerCrosshairUI.h"
#include "PlayerMainUI.h"
#include "UserWidgetBase.h"

// DEFINE_LOG_CATEGORY(LogUI);

void APlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();

	
	if (GetNetMode() == ENetMode::NM_Client)
	{
		if (CrosshairClass != nullptr)
		{
			CrosshairUI = CreateWidget<UPlayerCrosshairUI>(this, CrosshairClass);
			if (IsValid(CrosshairUI))
			{
				CrosshairUI->AddToViewport();
			}
		}
	}
}

void APlayerControllerBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	CreateMainUI();
}

void APlayerControllerBase::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
}

void APlayerControllerBase::CreateMainUI()
{
	if (MainUI)
	{
		UE_LOG(LogUI, Warning, TEXT("MainUI has already created"));
		return;
	}

	if (!IsLocalPlayerController())
	{
		UE_LOG(LogUI, Warning, TEXT("Current controller is not local player controller"));
		return;
	}

	if (!MainUIClass)
	{
		UE_LOG(LogUI, Warning, TEXT("MainUIClass is not configured"));
		return;
	}

	APlayerStateBase* PS = GetPlayerState<APlayerStateBase>();
	if (IsValid(PS) == false)
	{
		UE_LOG(LogUI, Warning, TEXT("Not found player state"));
		return;
	}

	MainUI = CreateWidget<UPlayerMainUI>(this, MainUIClass);
	if (MainUI)
	{
		MainUI->AddToViewport();
	}

	// Setting init data
	MainUI->SetHealth(PS->GetHealth(), PS->GetMaxHealth());
	MainUI->SetArmor(PS->GetArmor(), PS->GetMaxArmor());
	MainUI->SetCurrentAmmoInClip(PS->GetCurrentAmmoInClip());
	MainUI->SetTotalCarriedAmmo(PS->GetTotalCarriedAmmo());
}
