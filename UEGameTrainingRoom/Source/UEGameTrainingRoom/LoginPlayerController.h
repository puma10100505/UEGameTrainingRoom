// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LoginPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class UEGAMETRAININGROOM_API ALoginPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	virtual void ClientTravelToMap(const FString& URL, enum ETravelType TravelType);
};
