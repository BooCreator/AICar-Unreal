// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

#define DEBUGMESSAGE(x, ...) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, FString::Printf(TEXT(x), __VA_ARGS__));}

void AMyPlayerController::setMaxCarCount(int input)
{
	if (input >= 0)
		this->maxCarCount = input;
}

void AMyPlayerController::MoveForward(float Val)
{
	UE_LOG(LogTemp, Warning, TEXT("car %s id %i MoveForward %f"), *GetName(), (int)id, Val);
	car->MoveForward((float)Val);
}

void AMyPlayerController::MoveRight(float Val)
{
	UE_LOG(LogTemp, Warning, TEXT("car %s MoveRight %f"), *GetName(), Val);
	car->MoveRight((float)Val);
}

void AMyPlayerController::OnReset()
{
	car->SetActorRotation(FRotator(0, 0, 0));
	DEBUGMESSAGE("Car rotation was reset!")
}

AMyPlayerController::AMyPlayerController()
{

	// Добавить обработчик нажатия кнопки пробела
	//InputComponent->BindAction("Jump", IE_Pressed, this, &AMyPlayerController::OnJumpPressed);
/*
	FVector x(1, 0, 0);
	FVector y(0, 1, 0);
	FVector z(0, 0, 1);



	TArray <AAICarPawn*> myCars;
	for (int i = 0; i < maxCarCount; i++)
	{
		FVector Location(FMath::FRandRange(-maxX_AreaOfSpawn, maxX_AreaOfSpawn), FMath::FRandRange(-maxX_AreaOfSpawn, maxX_AreaOfSpawn), 200.0f);
		FRotator Rotation(0, FMath::FRandRange(0, 360), 0.0f);
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Owner = this;
		auto newCar = GetWorld()->SpawnActor<AAICarPawn>(Location, Rotation, SpawnInfo);
		if (newCar)
		{
			DEBUGMESSAGE("Spawned car %d, name %s", i, *newCar->GetName());
			myCars.Add(newCar);
			//myCars[0]->GetVehicleMovementComponent()->SetSteeringInput(0.5);
		}
	}
	
	//Debug part
	FRotator r = myCars[0]->GetActorRotation();
	UE_LOG(LogTemp, Warning, TEXT("%s"), *r.ToString());
	FVector px = r.RotateVector(x);
	FVector py = r.RotateVector(y);
	FVector pz = r.RotateVector(z);
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *SpawnInfo.Name.NameToDisplayString);		
	UE_LOG(LogTemp, Warning, TEXT("px:%s"), *px.ToString());
	UE_LOG(LogTemp, Warning, TEXT("py:%s"), *py.ToString());
	UE_LOG(LogTemp, Warning, TEXT("pz:%s"), *pz.ToString());*/

	//FVector pLoc = r.RotateVector(Location);
	//UE_LOG(LogTemp, Warning, TEXT("pLoc:%s"), *pLoc.ToString());
	//FVector LinearVelocity = myCars[0]->GetMesh()->GetPhysicsLinearVelocity();
	//UE_LOG(LogTemp, Warning, TEXT("LinearVelocity:%s"), *LinearVelocity.ToString());
	//-----------
}

void AMyPlayerController::SlowTick()
{
	//forward = FMath::FRandRange(-1, 1);
	//right = FMath::FRandRange(-1, 1);
}

void AMyPlayerController::setForward(double input)
{
	this->forward = input;
}

void AMyPlayerController::setRight(double input)
{
	this->right = input;
}

void AMyPlayerController::pause(bool bIsPausedInput)
{
	bIsPaused = bIsPausedInput;
	SetPause(bIsPausedInput);
}

void AMyPlayerController::RestoreUpright()
{
	/*FRotator Rotation = car->GetActorRotation();

	// Calculate the rotation needed to restore the car to its original state
	FRotator RestoreRotation = FRotator(0, 0, 0);

	// Set the rotation of the car
	car->SetActorRotation(RestoreRotation);
	car->SetActorRelativeRotation(RestoreRotation);

	car->SetActorRelativeLocation(FVector(0, 0, 500));

	DEBUGMESSAGE("Car rotation was reset!")*/
}

void AMyPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
	

	car->MoveForward(forward);
	car->MoveRight(right);

	//auto rot = car->GetActorRotation();
	//if (rot.Pitch > 30 || rot.Roll > 30)
	//	RestoreUpright();
/*
	if (id <= maxCarCount)
	{
		switch (id)
		{
		case 0:
			MoveForward(-1);
			//UE_LOG(LogTemp, Warning, TEXT("MoveForward car0"));
			break;
		case 1:
			if (counter <= maxCounter)
			{
				car->AddMovementInput(ForwardVector1);
				UE_LOG(LogTemp, Warning, TEXT("+++car1: ForwardVector:%s"), *ForwardVector1.ToString());
				counter++;
			}
			else
			{
				car->AddMovementInput(ForwardVector2);
				UE_LOG(LogTemp, Warning, TEXT("+++car1: ForwardVector:%s"), *ForwardVector2.ToString());
			}
			
			car->MoveForward(0.3);
			
			break;
		}
	}
	*/

	/*if (myCars.Num() <= maxCarCount)
	{
		FVector Location(FMath::FRandRange(-maxX_AreaOfSpawn, maxX_AreaOfSpawn), FMath::FRandRange(-maxX_AreaOfSpawn, maxX_AreaOfSpawn), 200.0f);
		FRotator Rotation(0, FMath::FRandRange(0, 360), 0.0f);
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Owner = this;
		auto newCar = GetWorld()->SpawnActor<AAICarPawn>(Location, Rotation, SpawnInfo);
		if (newCar)
		{
			DEBUGMESSAGE("Spawned car %d, name %s", myCars.Num(), *newCar->GetName());
			newCar->GetController();
			newCar->MoveForward(0.5);
			myCars.Add(newCar);
		}
	}*/

	if (currentForSlowCounter < delayForSlowCounter)
		currentForSlowCounter++;
	else
	{
		currentForSlowCounter = 0;
		SlowTick();
	}
}