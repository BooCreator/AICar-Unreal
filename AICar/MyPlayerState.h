// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class AICAR_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()	
public:
	virtual void Tick(float delay) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Player State")
	float CarHealth;
};
