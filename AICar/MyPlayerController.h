// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AICarPawn.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"


/**
 * 
 */
UCLASS()
class AICAR_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AMyPlayerController();
	void setCar(AAICarPawn* input) { if (input >= 0) car = input; };
	void setId(int input) { if(input >= 0) id = input; };
	void setMaxCarCount(int input);
	void MoveForward(float Val);
	void MoveRight(float Val);
	void SlowTick();
	void setForward(double input);
	void setRight(double input);
	void pause(bool bIsPausedInput);
	void RestoreUpright();

	bool bIsPaused = false;
	//void ControlCars(TArray<AAICarPawn*> CarsToControl);
	virtual void Tick(float delay) override;
	//virtual void BeginPlay() override;
private:
	int id = 0;
	int maxCarCount = 0;
	AAICarPawn* car;
	bool once=false;
	int counter = 0;
	int maxCounter = 1500;
	float forward = 0.0;
	float right = 0.0;
protected:
	// Обработчик нажатия кнопки пробела
	void OnReset();
	int currentForSlowCounter = 0;
	int delayForSlowCounter = 100;

};
