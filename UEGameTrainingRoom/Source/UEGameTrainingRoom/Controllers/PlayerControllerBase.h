// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"

#include "PlayerControllerBase.generated.h"

// DECLARE_LOG_CATEGORY_EXTERN(LogUI, Log, All);

/**
 * 
 */
UCLASS()
class UEGAMETRAININGROOM_API APlayerControllerBase 
	: public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|UI")
	TSubclassOf<class UPlayerCrosshairUI> CrosshairClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|UI")
	TSubclassOf<class UPlayerMainUI> MainUIClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGenericTeamId TeamId;

public:
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* aPawn) override;

	void CreateMainUI();

	void OnRep_PlayerState() override;

	FORCEINLINE class UPlayerMainUI* GetMainUI() 
	{
		return MainUI;
	}

protected:
	FORCEINLINE FGenericTeamId GetGenericTeamId() const { return TeamId; }

protected:
	UPROPERTY(BlueprintReadWrite, Category = CharacterUI)
	class UPlayerCrosshairUI* CrosshairUI;

	UPROPERTY(BlueprintReadWrite, Category = CharacterUI)
	class UPlayerMainUI* MainUI;
};
