// Copyright Epic Games, Inc. All Rights Reserved.

#include "AICarPawn.h"
#include "AICarWheelFront.h"
#include "AICarWheelRear.h"
#include "AICarHud.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/TextRenderComponent.h"
#include "Materials/Material.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "PhysicsEngine/PhysicsSettings.h"

//#include "WheeledVehicleMovementComponent.h"

#ifndef HMD_MODULE_INCLUDED
#define HMD_MODULE_INCLUDED 0
#endif

// Needed for VR Headset
#if HMD_MODULE_INCLUDED
#include "IXRTrackingSystem.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#endif // HMD_MODULE_INCLUDED
#include <Runtime/Engine/Classes/Components/WindDirectionalSourceComponent.h>

const FName AAICarPawn::LookUpBinding("LookUp");
const FName AAICarPawn::LookRightBinding("LookRight");

#define LOCTEXT_NAMESPACE "VehiclePawn"
#define MM 100000
#define DEBUGMESSAGE(x, ...) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT(x), __VA_ARGS__));}

#define M_PI           3.14159265358979323846  /* pi */

PRAGMA_DISABLE_DEPRECATION_WARNINGS

void AAICarPawn::saveFloatSetting(FString section, FString param, float value)
{
	GConfig->SetFloat(*section, *param, value, GGameIni);
}

float AAICarPawn::readFloatSetting(FString section, FString param)
{
	float value;
	if (GConfig->GetFloat(*section, *param, value, GGameIni))
	{
		UE_LOG(LogTemp, Warning, TEXT("read from ini : %d"), value);
	}
	return value;
}

void AAICarPawn::saveIntSetting(FString section, FString param, int value)
{
	GConfig->SetInt(*section, *param, value, GGameIni);
}

int AAICarPawn::readIntSetting(FString section, FString param)
{
	int value;
	if (GConfig->GetInt(*section, *param, value, GGameIni))
	{
		UE_LOG(LogTemp, Warning, TEXT("read from ini : %d"), value);
	}
	return value;
}

void AAICarPawn::saveStringSetting(FString section, FString param, FString value)
{
	GConfig->SetString(*section, *param, *value, GGameIni);
}

FString AAICarPawn::readStringSetting(FString section, FString param)
{
	FString temp;
	if (GConfig->GetString(*section, *param, temp, GGameIni))
	{
		UE_LOG(LogTemp, Warning, TEXT("read from ini : %s"), *temp);
	}
	return temp;
}

void AAICarPawn::saveBoolSetting(FString section, FString param, bool value)
{
	GConfig->SetBool(*section, *param, value, GGameIni);
}

AAICarPawn::AAICarPawn()
{
	//DEBUGMESSAGE("AAICarPawn car name %s",*GetName());
	// Car mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CarMesh(TEXT("/Game/Vehicle/Sedan/Sedan_SkelMesh.Sedan_SkelMesh"));
	GetMesh()->SetSkeletalMesh(CarMesh.Object);

	static ConstructorHelpers::FClassFinder<UObject> AnimBPClass(TEXT("/Game/Vehicle/Sedan/Sedan_AnimBP"));
	GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);
	
	// Simulation
	UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

	check(Vehicle4W->WheelSetups.Num() == 4);

	Vehicle4W->WheelSetups[0].WheelClass = UAICarWheelFront::StaticClass();
	Vehicle4W->WheelSetups[0].BoneName = FName("Wheel_Front_Left");
	Vehicle4W->WheelSetups[0].AdditionalOffset = FVector(0.f, -12.f, 0.f);

	Vehicle4W->WheelSetups[1].WheelClass = UAICarWheelFront::StaticClass();
	Vehicle4W->WheelSetups[1].BoneName = FName("Wheel_Front_Right");
	Vehicle4W->WheelSetups[1].AdditionalOffset = FVector(0.f, 12.f, 0.f);

	Vehicle4W->WheelSetups[2].WheelClass = UAICarWheelRear::StaticClass();
	Vehicle4W->WheelSetups[2].BoneName = FName("Wheel_Rear_Left");
	Vehicle4W->WheelSetups[2].AdditionalOffset = FVector(0.f, -12.f, 0.f);

	Vehicle4W->WheelSetups[3].WheelClass = UAICarWheelRear::StaticClass();
	Vehicle4W->WheelSetups[3].BoneName = FName("Wheel_Rear_Right");
	Vehicle4W->WheelSetups[3].AdditionalOffset = FVector(0.f, 12.f, 0.f);

	// Create a spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->TargetOffset = FVector(0.f, 0.f, 200.f);
	SpringArm->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 600.0f;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraRotationLagSpeed = 7.f;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;

	// Create camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
	Camera->FieldOfView = 90.f;

	// Create In-Car camera component 
	InternalCameraOrigin = FVector(0.0f, -40.0f, 120.0f);

	InternalCameraBase = CreateDefaultSubobject<USceneComponent>(TEXT("InternalCameraBase"));
	InternalCameraBase->SetRelativeLocation(InternalCameraOrigin);
	InternalCameraBase->SetupAttachment(GetMesh());

	InternalCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("InternalCamera"));
	InternalCamera->bUsePawnControlRotation = false;
	InternalCamera->FieldOfView = 90.f;
	InternalCamera->SetupAttachment(InternalCameraBase);

	//Setup TextRenderMaterial
	static ConstructorHelpers::FObjectFinder<UMaterial> TextMaterial(TEXT("Material'/Engine/EngineMaterials/AntiAliasedTextMaterialTranslucent.AntiAliasedTextMaterialTranslucent'"));
	
	UMaterialInterface* Material = TextMaterial.Object;

	// Create text render component for in car speed display
	InCarSpeed = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarSpeed"));
	InCarSpeed->SetTextMaterial(Material);
	InCarSpeed->SetRelativeLocation(FVector(70.0f, -75.0f, 99.0f));
	InCarSpeed->SetRelativeRotation(FRotator(18.0f, 180.0f, 0.0f));
	InCarSpeed->SetupAttachment(GetMesh());
	InCarSpeed->SetRelativeScale3D(FVector(1.0f, 0.4f, 0.4f));

	// Create text render component for in car gear display
	InCarGear = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarGear"));
	InCarGear->SetTextMaterial(Material);
	InCarGear->SetRelativeLocation(FVector(66.0f, -9.0f, 95.0f));	
	InCarGear->SetRelativeRotation(FRotator(25.0f, 180.0f,0.0f));
	InCarGear->SetRelativeScale3D(FVector(1.0f, 0.4f, 0.4f));
	InCarGear->SetupAttachment(GetMesh());
	
	// Colors for the incar gear display. One for normal one for reverse
	GearDisplayReverseColor = FColor(255, 0, 0, 255);
	GearDisplayColor = FColor(255, 255, 255, 255);

	// Colors for the in-car gear display. One for normal one for reverse
	GearDisplayReverseColor = FColor(255, 0, 0, 255);
	GearDisplayColor = FColor(255, 255, 255, 255);

	bInReverseGear = false;

	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
	w_area = readFloatSetting("car", "w_area");
	h_area = readFloatSetting("car", "h_area");
	debug = readBoolSetting("car", "debug");
	padding = readFloatSetting("car", "padding");
	cameraMainNeed = readBoolSetting("car", "cameraMainNeed");
	locationOfInternalCamera = readStringSetting("car", "location_of_internal_camera");
	rotationOfInternalCamera = readStringSetting("car", "rotation_of_internal_camera");
	nameOfCamera = readStringSetting("car", "nameMainCamera");
	minDistanceToTarget = readFloatSetting("car", "minDistanceToTarget")*MM;
	currentCameraNum = readFloatSetting("car", "lastCamera");

	// Создать строку
	FString LocationString = "10.0, 20.0, 30.0";
	FString RotationString = "45.0, 0.0, 0.0";

	// Создать вектор
	FVector Location;
	Location.InitFromString(locationOfInternalCamera);

	// Создать вращение
	FRotator Rotation;
	Rotation.InitFromString(rotationOfInternalCamera);

	// Установить относительное положение камеры
	InternalCamera->SetRelativeLocation(Location);

	// Установить относительное вращение камеры
	InternalCamera->SetRelativeRotation(Rotation);

	// Создать вектор
	/*FVector internalCameraLocation = FVector::InitFromString(locationOfInternalCamera);
	FRotator internalCameraRotation;

	// Инициализировать вектор из строки
	InternalCamera->SetRelativeLocation();
	InternalCamera->SetRelativeRotation(internalCameraRotation.InitFromString(locationOfInternalCamera));
	*/

	if (cameraMainNeed)
		currentCameraNum = 2;
	loadColumnCounter();
	startTimer();
    
    if(!beginCatchTargetTime)
		beginCatchTargetTime = GetLastRenderTime();

	float drag_coefficient = readFloatSetting("car", "drag_coefficient");
	if (!drag_coefficient)
	{
		saveFloatSetting("car", "drag_coefficient", 0.3);
		drag_coefficient = readFloatSetting("car", "drag_coefficient");
	}
	UE_LOG(LogTemp, Warning, TEXT("drag_coefficient = %f"), drag_coefficient);
	GetVehicleMovementComponent()->DragCoefficient = drag_coefficient;

	MainBodyInstance = NULL;

}

void AAICarPawn::toogleAutopilot()
{
	autopilot = !autopilot;
}

double AAICarPawn::startTimer()
{
	start = FPlatformTime::Seconds();
	return start;
}

double AAICarPawn::stopTimer()
{
	end = FPlatformTime::Seconds();
	return end;
}

void AAICarPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PrimaryActorTick.bCanEverTick = true;
	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AAICarPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AAICarPawn::MoveRight);

	PlayerInputComponent->BindAxis("MoveForward", this, &AAICarPawn::MoveForwardManual);
	PlayerInputComponent->BindAxis("MoveRight", this, &AAICarPawn::MoveRightManual);
	

	PlayerInputComponent->BindAxis("LookUp");
	PlayerInputComponent->BindAxis("LookRight");

	PlayerInputComponent->BindAction("Handbrake", IE_Pressed, this, &AAICarPawn::OnHandbrakePressed);
	PlayerInputComponent->BindAction("Handbrake", IE_Released, this, &AAICarPawn::OnHandbrakeReleased);
	PlayerInputComponent->BindAction("SwitchCamera", IE_Pressed, this, &AAICarPawn::OnToggleCamera);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AAICarPawn::OnResetVR); 
	PlayerInputComponent->BindAction("ResetGame", IE_Pressed, this, &AAICarPawn::RestartGame);
	PlayerInputComponent->BindAction("ExitGame", IE_Pressed, this, &AAICarPawn::ExitGame);
	PlayerInputComponent->BindAction("Autopilot", IE_Pressed, this, &AAICarPawn::toogleAutopilot);
	PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &AAICarPawn::eventPause);
}

void AAICarPawn::eventPause(){
	bIsPaused = !bIsPaused;
}

bool AAICarPawn::getStatePause()
{
	return bIsPaused;
}

void AAICarPawn::MoveForward(float Val)
{
	if (GetName() == "TP_VehiclePawn_9" && autopilot)
		GetVehicleMovementComponent()->SetThrottleInput(Val);
	else if (GetName() != "TP_VehiclePawn_9")
		GetVehicleMovementComponent()->SetThrottleInput(Val);
}

void AAICarPawn::MoveForwardManual(float Val)
{
	GetVehicleMovementComponent()->SetThrottleInput(Val);
}

void AAICarPawn::MoveRight(float Val)
{
	if (GetName() == "TP_VehiclePawn_9" && autopilot)
		GetVehicleMovementComponent()->SetSteeringInput(Val);
	else if (GetName() != "TP_VehiclePawn_9")
		GetVehicleMovementComponent()->SetSteeringInput(Val);
}

void AAICarPawn::MoveRightManual(float Val)
{
	GetVehicleMovementComponent()->SetSteeringInput(Val);
}

void AAICarPawn::MoveWithVector(FVector Velocity)
{
	// Нормализуем вектор скорости
	Velocity.Normalize();
}

void AAICarPawn::OnHandbrakePressed()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

void AAICarPawn::OnHandbrakeReleased()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

void AAICarPawn::OnToggleCamera()
{
	EnableIncarView();
}

void AAICarPawn::EnableIncarView( const bool bForce)
{
	nameOfCamera = readStringSetting("car", "nameMainCamera");
	auto newCamera = FindActorByName(GetWorld(), nameOfCamera);//"MyCamera_0");
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	prevView = PlayerController->GetViewTarget();
	saveIntSetting("car", "lastCamera", currentCameraNum);
	switch (currentCameraNum)
	{
	case 0:
		PlayerController->SetViewTarget(FindActorByName(GetWorld(), "TP_VehiclePawn_9"));
			InternalCamera->Deactivate();
			Camera->Activate();
			InCarSpeed->SetVisibility(false);
			InCarGear->SetVisibility(false);
			//if (debug)
				//DEBUGMESSAGE("Camera 0");
		break;
	case 1:
			Camera->Deactivate();			
			InternalCamera->Activate();
			UE_LOG(LogTemp, Warning, TEXT("Internal camera location : %s"), *InternalCamera->GetRelativeLocation().ToString());
			InCarSpeed->SetVisibility(true);
			InCarGear->SetVisibility(true);
			//if (debug)
			//	DEBUGMESSAGE("Camera 1");
		break;
	case 2:
		InternalCamera->Deactivate();
		Camera->Deactivate();

		//newCamera->GetRootComponent()->Activate();

		// Подключаемся к виду новой камеры
		PlayerController->SetViewTarget(newCamera);
		//if (debug)
		//	DEBUGMESSAGE("Camera 2");
		currentCameraNum = -1;
		break;
	default:
		currentCameraNum = -1;
		if (debug)
			DEBUGMESSAGE("Camera reset");
		break;
	}
	currentCameraNum++;
}

double AAICarPawn::distancePoints(int x1,int y1,int x2,int y2)
{
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void AAICarPawn::trimCoord(double& x, double& y, double r)
{
	if (x <= (-r + padding * MM))
	{
		x = -r + padding * MM;
		UE_LOG(LogTemp, Warning, TEXT("Replace1, x = %f, y = %f"), x, y);
	}
	if (x >= (r - padding * MM))
	{
		x = r - padding * MM;
		UE_LOG(LogTemp, Warning, TEXT("Replace2, x = %f, y = %f"), x, y);
	}
	if (y <= (-r + padding * MM))
	{
		y = -r + padding * MM;
		UE_LOG(LogTemp, Warning, TEXT("Replace3, x = %f, y = %f"), x, y);
	}
	if (y >= (r - padding * MM))
	{
		y = r - padding * MM;
		UE_LOG(LogTemp, Warning, TEXT("Replace4, x = %f, y = %f"), x, y);
	}
}

FVector AAICarPawn::nextCoord(FVector prevPos)
{
	// Ширина и высота зоны
	double width = w_area * MM;
	double height = h_area * MM;

	// Координаты предыдущего шара
	double x_old = prevPos.X;
	double y_old = prevPos.Y;

	int r = width/2;
	double t;

	double x;
	double y;

	int i = 0;

	float biggestDistance = 0;
	double biggestX=0;
	double biggestY=0;
	bool correct = false;

	srand((unsigned)time(NULL));
	// Проверка, что координаты следующего шара находятся внутри зоны
	for (i=0; i < 100; i++) {
		// Генерация случайного угла поворота
		//t = rand() * 2 * M_PI;
		t = FMath::FRandRange(0, 360)*PI/180;
		// Расчет координат случайной точки
		x = r * cos(t) + x_old;
		y = r * sin(t) + y_old;
		double Distance = distancePoints(prevPos.X, prevPos.Y, x, y);

			double x1 = x;
			double y1 = y;
			trimCoord(x1, y1, r);
			double Distance1 = distancePoints(prevPos.X, prevPos.Y, x1, y1);

		if (biggestDistance < Distance1)
		{   
			
			biggestDistance = Distance;
			biggestX = x1;
			biggestY = y1;
		}

		UE_LOG(LogTemp, Warning, TEXT("Try, i = %i Distance = %f x = %f y = %f. Bounds x_min = %f, x_max = %f, y_min = %f, y_max = %f"), i, Distance, x, y,
			(-r + padding * MM), (r - padding * MM), (-height / 2 + padding * MM), (height / 2 - padding * MM));
		if ((x >= (-r + padding * MM)) && (x <= (r - padding * MM)) && (y > (-height / 2 + padding * MM)) && (y < (height / 2 - padding * MM)))
		{
			if (Distance > (r - 100) && Distance < (r + 100))
			{
				UE_LOG(LogTemp, Warning, TEXT("Complete task, i = %i"), i);
				correct = true;
				break;
			}
		}
		else
		{
			x = biggestX;
			y = biggestY;
		}
	}

	if(correct)
		trimCoord(x, y, r);
	else
	{
		x = biggestX;
		y = biggestY;
	}

	double Distance = distancePoints(prevPos.X,prevPos.Y, x , y);

	/*if (Distance < r && !correct)
	{
		x = -x; y = -y;
		UE_LOG(LogTemp, Warning, TEXT("Reverce, x = %f, y = %f"), x, y);
	}*/

	UE_LOG(LogTemp, Warning, TEXT("GetDistanceTo(column) = %f, i = %i, carName = %s"), Distance, i, *GetName());

	return FVector(x, y, prevPos.Z);
}

void AAICarPawn::createINI()
{
	saveFloatSetting("car", "maxCarCount", 15);
	saveFloatSetting("car", "w_area", 0.32);
	saveFloatSetting("car", "h_area", 0.24);
	saveIntSetting("car", "cameraMainNeed", 0);
	saveFloatSetting("car", "padding", 0.03);
	saveFloatSetting("car", "additionalHeightOfCamera", 1);

	saveStringSetting("car", "counter_column", "0/7");
	saveFloatSetting("car", "minDistanceToTarget", 0.004);
	saveFloatSetting("car", "suspension_damping_ratio", 5);
	saveFloatSetting("car", "lat_stiff_max_load", 2);
	saveFloatSetting("car", "drag_coefficient", 0.3);
	saveStringSetting("car", "location_of_internal_camera", "X=0,Y=-12,Z=10");
	saveStringSetting("car", "rotation_of_internal_camera", "P=0,Y=5,R=7");
	saveIntSetting("car", "debug", 0);
	saveStringSetting("car", "nameMainCamera", "Camera_main");
	saveFloatSetting("car", "lastCamera", 3);

	saveFloatSetting("ai_cars", "count", 1);
	saveStringSetting("ai_cars", "AI_0", "Type=bus,Color=black,Num=5");
	saveStringSetting("ai_cars", "AI_1", "Type=bus,Color=yellow,Num=5");
}

void AAICarPawn::loadColumnCounter()
{
	FString counter_column = readStringSetting("car", "counter_column");
	if (counter_column.IsEmpty())
	{
		createINI();
		counter_column = readStringSetting("car", "counter_column");
	}
	TArray<FString> Parts;
	counter_column.ParseIntoArray(Parts, TEXT("/"), true);
	counter_column_current = FCString::Atoi(*Parts[0]);
	counter_column_max = FCString::Atoi(*Parts[1]);
}

void AAICarPawn::SlowTick(bool isFinish)
{
	if (isFinish)
	{
		historyLocation.emplace_back(GetActorLocation());
		if (historyLocation.size() > maxSizeHistory)
			historyLocation.clear();

		distanceOfCar = 0;
		for (int i = 1; i < historyLocation.size(); i=i+2)
		{
			distanceOfCar += FVector::Distance(historyLocation[i - 1], historyLocation[i]);
		}
	}
}

void AAICarPawn::setNameOfCamera(FString input)
{
	if(input.Len())
		nameOfCamera = input;
}

void AAICarPawn::Tick(float Delta)
{
	Super::Tick(Delta);

	// Add custom physics on MainBodyMesh
	if (MainBodyInstance != NULL) {
		MainBodyInstance->AddCustomPhysics(OnCalculateCustomPhysics);
	}


	PrimaryActorTick.bCanEverTick = true;
	// Setup the flag to say we are in reverse gear
	bInReverseGear = GetVehicleMovement()->GetCurrentGear() < 0;
	
	// Update the strings used in the hud (incar and onscreen)
	UpdateHUDStrings();

	// Set the string in the incar hud
	SetupInCarHUD();

	bool bHMDActive = false;
#if HMD_MODULE_INCLUDED
	if ((GEngine->XRSystem.IsValid() == true) && ((GEngine->XRSystem->IsHeadTrackingAllowed() == true) || (GEngine->IsStereoscopic3D() == true)))
	{
		bHMDActive = true;
	}
#endif // HMD_MODULE_INCLUDED
	if (bHMDActive == false)
	{
		if ( (InputComponent) && (bInCarCameraActive == true ))
		{
			FRotator HeadRotation = InternalCamera->GetRelativeRotation();
			HeadRotation.Pitch += InputComponent->GetAxisValue(LookUpBinding);
			HeadRotation.Yaw += InputComponent->GetAxisValue(LookRightBinding);
			InternalCamera->SetRelativeRotation(HeadRotation);
		}
	}
	//pos = Vec(GetActorLocation().X, GetActorLocation().Y);
	//GetDistanceTo()
	
	TArray<AActor*> column;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMainTargetActor::StaticClass(), column);

	if(column.Num())
	{
		//if(debug)
		//	DEBUGMESSAGE("Find column, name %s", *GetName());
		double columnDistance = GetDistanceTo(column[0]);
		//UE_LOG(LogTemp, Warning, TEXT("GetDistanceTo(column) = %f"), columnDistance);
		if (columnDistance < minDistanceToTarget)
		{
			//if (debug)
			//	DEBUGMESSAGE("columnDistance = %f", columnDistance);
			if (GetName() == "TP_VehiclePawn_9")
			{
				//UGameplayStatics::SpawnSoundAtLocation(this, PicketSound, GetActorLocation());

				//if (debug)
				//	DEBUGMESSAGE("TP_VehiclePawn_9 near column, name %s", *GetName());
				
				auto currentLoc = column[0]->GetActorLocation();
				FVector newLocation = nextCoord(currentLoc);
				column[0]->SetActorLocation(newLocation);
				loadColumnCounter();

				counter_column_current++;
				if (counter_column_current >= counter_column_max)
				{
					/*// Получить объект музыкального игрока
					AActor* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
					UAudioComponent* AudioComponent = PlayerPawn->GetAudioComponent();

					// Получить мелодию из стартового контента
					USoundWave* SoundWave = LoadObject<USoundWave>(nullptr, TEXT("/Game/StarterContent/Audio/Music/MenuTheme"));

					// Воспроизвести мелодию
					AudioComponent->PlaySound(SoundWave, 1.0f, 1.0f, 0.0f);*/
					/*auto OnOverlapSound = CreateDefaultSubobject<UAudioComponent>(TEXT("PropellerAudioComp"));
					OnOverlapSound->bAutoActivate = false;
					//OnOverlapSound->AttachTo(MeshComponent);
					static ConstructorHelpers::FObjectFinder<USoundBase> soundBase(
						TEXT("/Game/StarterContent/Audio/Collapse01.Collapse01")
					);
					OnOverlapSound->SetSound(soundBase.Object);
					OnOverlapSound->Play();
					OnOverlapSound->SetFloatParameter(FName("pitch"), 2500.f);*/


					DEBUGMESSAGE("All target are reached %i/%i", counter_column_current, counter_column_max);
					//counter_column_current = 0;
					FString resultText = FString::Printf(TEXT("%i/%i" ), counter_column_current, counter_column_max);
					saveStringSetting("car", "counter_column", resultText);
					if (currentCameraNum == 0)	//костыль для работы камеры после рестарта
						currentCameraNum = 3;
					if(currentCameraNum != 2)
						saveIntSetting("car", "cameraMainNeed", 0);
					else if (currentCameraNum == 3)
						saveIntSetting("car", "cameraMainNeed", 1);
					else
						saveIntSetting("car", "cameraMainNeed", 1);

					saveIntSetting("car", "lastCamera", --currentCameraNum);

					//RestartGame();
					needGameOver = true;
                    if (!endCatchTargetTime)
						endCatchTargetTime = GetLastRenderTime();
				}
				FString resultText = FString::Printf(TEXT("%i/%i" ), counter_column_current, counter_column_max);
				saveStringSetting("car", "counter_column", resultText);

				stopTimer();
				arrTime.emplace_back(end - start);
				countOfMainColumn++;
				startTimer();
			}
		}
	}

	if (targetActor)
	{
		double distanceToTarget = GetDistanceTo(targetActor);
		if (distanceToTarget < minDistanceToTarget)
		{
			auto currentLoc = targetActor->GetActorLocation();
			FVector newLocation = nextCoord(currentLoc);
			targetActor->SetActorLocation(newLocation);
			stopTimer();
			arrTime.emplace_back(end - start);
			countOfColumn++;
			startTimer();
		}
	}

	//auto rot = GetActorRotation();
	//if (rot.Pitch > 30 || rot.Roll > 30)
	//	RestoreUpright();

	if (currentForSlowCounter < delayForSlowCounter)
	{
		currentForSlowCounter++;
		SlowTick(false);
	}
	else
	{
		currentForSlowCounter = 0;
		SlowTick(true);
	}
}

void AAICarPawn::RestartGame()
{
	counter_column_current = 0;
	FString resultText = FString::Printf(TEXT("%i/%i"), counter_column_current, counter_column_max);
	saveStringSetting("car", "counter_column", resultText);

	UWorld* World = GetWorld();

	if (World)
	{
		// Используем функцию RestartLevel() для перезапуска уровня
		UGameplayStatics::OpenLevel(World, *World->GetName(), true);
	}
	DEBUGMESSAGE("RestartGame");
}

void AAICarPawn::ExitGame()
{
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();

	if (GameInstance)
	{
		// Выполняем команду "quit" для выключения игры
		UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, true);
	}
}

// Функция для поиска актора по имени
AActor* AAICarPawn::FindActorByName(UWorld* World, FString ActorName)
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), Actors);

	for (AActor* Actor : Actors)
	{
		if (Actor->GetName() == ActorName)
		{
			return Actor;
		}
	}

	return nullptr;
}

void AAICarPawn::BeginPlay()
{
	Super::BeginPlay();
	PrimaryActorTick.bCanEverTick = true;
	bool bEnableInCar = false;
#if HMD_MODULE_INCLUDED
	bEnableInCar = UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled();
#endif // HMD_MODULE_INCLUDED
	EnableIncarView(true);

	if (GetMesh() != NULL) {
		MainBodyInstance = GetMesh()->GetBodyInstance();
	}

}

void AAICarPawn::OnResetVR()
{
#if HMD_MODULE_INCLUDED
	if (GEngine->XRSystem.IsValid())
	{
		GEngine->XRSystem->ResetOrientationAndPosition();
		InternalCamera->SetRelativeLocation(InternalCameraOrigin);
		GetController()->SetControlRotation(FRotator());
	}
#endif // HMD_MODULE_INCLUDED
}

void AAICarPawn::UpdateHUDStrings()
{
	float KPH = FMath::Abs(GetVehicleMovement()->GetForwardSpeed()) * 0.036f;
	int32 KPH_int = FMath::FloorToInt(KPH);

	// Using FText because this is display text that should be localizable
	SpeedDisplayString = FText::Format(LOCTEXT("SpeedFormat", "{0} km/h"), FText::AsNumber(KPH_int));
	
	if (bInReverseGear == true)
	{
		GearDisplayString = FText(LOCTEXT("ReverseGear", "R"));
	}
	else
	{
		int32 Gear = GetVehicleMovement()->GetCurrentGear();
		GearDisplayString = (Gear == 0) ? LOCTEXT("N", "N") : FText::AsNumber(Gear);
	}	
}

void AAICarPawn::SetupInCarHUD()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if ((PlayerController != nullptr) && (InCarSpeed != nullptr) && (InCarGear != nullptr) )
	{
		// Setup the text render component strings
		InCarSpeed->SetText(SpeedDisplayString);
		InCarGear->SetText(GearDisplayString);
		
		if (bInReverseGear == false)
		{
			InCarGear->SetTextRenderColor(GearDisplayColor);
		}
		else
		{
			InCarGear->SetTextRenderColor(GearDisplayReverseColor);
		}
	}
}

void AAICarPawn::RestoreUpright()
{
	FRotator Rotation = GetActorRotation();

	// Calculate the rotation needed to restore the car to its original state
	FRotator RestoreRotation = FRotator(-Rotation.Pitch, Rotation.Yaw, Rotation.Roll);

	// Set the rotation of the car
	SetActorRotation(RestoreRotation);

	DEBUGMESSAGE("Car rotation was reset!")
}

bool AAICarPawn::readBoolSetting(FString section, FString param)
{
	bool value;
	if (GConfig->GetBool(*section, *param, value, GGameIni))
	{
		UE_LOG(LogTemp, Warning, TEXT("read from ini : %d"), value);
	}
	return value;
}
bool AAICarPawn::playSound()
{
	
	return true;
}

#undef LOCTEXT_NAMESPACE

PRAGMA_ENABLE_DEPRECATION_WARNINGS
