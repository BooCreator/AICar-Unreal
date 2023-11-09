// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerState.h"

void AMyPlayerState::Tick(float delay)
{
    Super::Tick(delay);
    UE_LOG(LogTemp, Warning, TEXT("Tick from AMyPlayerState"));
}
