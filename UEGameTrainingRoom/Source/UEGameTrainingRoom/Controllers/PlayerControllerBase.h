// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"

#include "PlayerControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class UEGAMETRAININGROOM_API APlayerControllerBase 
	: public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* aPawn) override;

protected:
	FORCEINLINE FGenericTeamId GetGenericTeamId() const { return TeamId; }

protected:
	UPROPERTY(BlueprintReadWrite, Category = CharacterUI)
	class UUserWidgetBase* CrosshairUI;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = CharacterUI)
	TSubclassOf<class UUserWidgetBase> CrosshairClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGenericTeamId TeamId;
};
