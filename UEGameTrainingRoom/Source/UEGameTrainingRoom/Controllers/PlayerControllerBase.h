// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class UEGAMETRAININGROOM_API APlayerControllerBase : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* aPawn) override;

protected:
	UPROPERTY(BlueprintReadWrite, Category = CharacterUI)
	class UUserWidgetBase* CrosshairUI;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = CharacterUI)
	TSubclassOf<class UUserWidgetBase> CrosshairClass;
};
