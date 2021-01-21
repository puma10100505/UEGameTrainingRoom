// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TrainingRoomGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class UEGAMETRAININGROOM_API UTrainingRoomGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UTrainingRoomGameInstance();

	UFUNCTION(BlueprintCallable, Category = "Replay")
	void StartRecording();

	UFUNCTION(BlueprintCallable, Category = "Replay")
	void StopRecording();

	UFUNCTION(BlueprintCallable, Category = "Replay")
	void StartReplay();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Replay")
	FString RecordingName;

	UPROPERTY(EditDefaultsOnly, Category = "Replay")
	FString FriendlyRecordingName;
};
