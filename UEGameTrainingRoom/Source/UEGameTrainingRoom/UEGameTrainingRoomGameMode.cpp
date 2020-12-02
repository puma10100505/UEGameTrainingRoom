// Copyright Epic Games, Inc. All Rights Reserved.

#include "UEGameTrainingRoomGameMode.h"
#include "UEGameTrainingRoomCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUEGameTrainingRoomGameMode::AUEGameTrainingRoomGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
