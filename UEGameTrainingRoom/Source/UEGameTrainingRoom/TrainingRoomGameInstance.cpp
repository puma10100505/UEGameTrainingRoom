// Fill out your copyright notice in the Description page of Project Settings.


#include "TrainingRoomGameInstance.h"


UTrainingRoomGameInstance::UTrainingRoomGameInstance()
{
    RecordingName = "TrainingRoomGame";
    FriendlyRecordingName = "Training Room";
}

void UTrainingRoomGameInstance::StartRecording()
{
    TArray<FString> URLOptions;
    URLOptions.Add("ReplayStreamerOverride=InMemoryNetworkReplayStreaming");
    StartRecordingReplay(RecordingName, FriendlyRecordingName);
}

void UTrainingRoomGameInstance::StopRecording()
{
    StopRecordingReplay();
}

void UTrainingRoomGameInstance::StartReplay()
{
    PlayReplay(RecordingName, nullptr);
}