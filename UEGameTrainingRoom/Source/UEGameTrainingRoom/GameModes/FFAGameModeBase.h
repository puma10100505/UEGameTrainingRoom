// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UEGameTrainingRoomGameMode.h"
#include "FFAGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class UEGAMETRAININGROOM_API AFFAGameModeBase : public AUEGameTrainingRoomGameMode
{
	GENERATED_BODY()
	
public:
	UFUNCTION(Exec)
	void GMDebug();
};
