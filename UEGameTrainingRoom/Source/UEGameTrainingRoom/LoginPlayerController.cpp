// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginPlayerController.h"

void ALoginPlayerController::ClientTravelToMap(const FString& URL, enum ETravelType TravelType)
{
    ClientTravel(URL, TravelType);
}