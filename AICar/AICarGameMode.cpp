// Copyright Epic Games, Inc. All Rights Reserved.

#include "AICarGameMode.h"
#include "AICarPawn.h"
#include "AICarHud.h"
#include "MyPlayerState.h"
#include <chrono>
#include <ctime> 
#include <vector>
#include "Wall.h"
#include "Floor.h"
#include "MyText.h"
#include "Engine/GameEngine.h"
#include "WheeledVehicleMovementComponent4W.h"

#define M_PI           3.14159265358979323846  /* pi */
#define DEBUGMESSAGE(x, ...) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, FString::Printf(TEXT(x), __VA_ARGS__));}
#define MM 100000
#define M 100

AAICarGameMode::AAICarGameMode()
{
	DefaultPawnClass = AAICarPawn::StaticClass();
	HUDClass = AAICarHud::StaticClass();
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;

	this->X1 = 0; this->Y1 = 0;
	this->X2 = w_area * MM; this->Y2 = h_area * MM;
	
	loadParams();
	this->initGame();
}

AAICarGameMode::~AAICarGameMode()
{
	
}


void AAICarGameMode::loadParams()
{
	maxCarCount = readFloatSetting("car", "maxCarCount") - 1;
	w_area = readFloatSetting("car", "w_area");
	h_area = readFloatSetting("car", "h_area");
	cameraMainNeed = readBoolSetting("car", "cameraMainNeed");
	padding = readFloatSetting("car", "padding");
	this->staticPosition = readFloatSetting("car", "use_static");
	additionalCoefHeightOfCamera = readFloatSetting("car", "additionalHeightOfCamera");
	this->ai_env.load_ai();
}

void AAICarGameMode::initGame()
{
	myCars.Empty();	
	myCarsController.Empty();
	targets.Empty();
	carInfoArr.Empty();
}

//void AAICarGameMode::StartPlay()
//{
	//GetWorldTimerManager().SetTimer(TimerHandle, this, &AAICarGameMode::TimerCallback, TimerDelay, true);
//}

void AAICarGameMode::createFloorAndWall()
{
	FVector LocationFloor(0.0f, 0.0f, 0.0f);
	FRotator RotationFloor(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnInfoFloor;
	SpawnInfoFloor.Owner = this;

	auto floor = GetWorld()->SpawnActor<AFloor>(LocationFloor, RotationFloor, SpawnInfoFloor);

	// ���������� ��� �������� ��������
	FVector Origin;
	FVector BoxExtent;
	// �������� ������� ������
	floor->GetActorBounds(false, Origin, BoxExtent);
	// ������ � ���������� BoxExtent � ��� ����� ������� ��������� � ���� ���������� (X, Y, Z)
	float Width = BoxExtent.X * 2.0f;
	float Height = BoxExtent.Y * 2.0f;
	float Depth = BoxExtent.Z * 2.0f;

    //this->env = Environment(Width, Height);
	maxCoordX = BoxExtent.X;
	maxCoordY = BoxExtent.Y;

	this->wallsInfo = {
		WallInfo(
			(-this->maxCoordX + this->wallsShift) / M, (-this->maxCoordY + this->wallsShift) / M,
			(this->maxCoordX - this->wallsShift)  / M, (-this->maxCoordY + this->wallsShift) / M,
			2.),
		WallInfo(
			(this->maxCoordX - this->wallsShift) / M, (-this->maxCoordY + this->wallsShift) / M,
			(this->maxCoordX - this->wallsShift) / M, (this->maxCoordY - this->wallsShift)  / M,
			2.),
		WallInfo(
			(this->maxCoordX - this->wallsShift)  / M,  (this->maxCoordY - this->wallsShift) / M,
			(-this->maxCoordX + this->wallsShift) / M, (this->maxCoordY - this->wallsShift)  / M,
			2.),
		WallInfo(
			(-this->maxCoordX + this->wallsShift) / M, (this->maxCoordY - this->wallsShift)  / M,
			(-this->maxCoordX + this->wallsShift) / M, (-this->maxCoordY + this->wallsShift) / M,
			2.),
	};

	FActorSpawnParameters SpawnInfo;

	for (float i = -maxCoordX; i < maxCoordX; i += 600)
	{
		FTransform SpawnLoc;
		SpawnLoc.SetLocation(FVector(i, -maxCoordY + this->wallsShift, 0));
		SpawnLoc.SetRotation(FQuat(FRotator(0, 0, 0)));
		SpawnLoc.SetScale3D(FVector(1, 1, 1));
		AActor* CurrentFlora = GetWorld()->SpawnActor<AActor>(AWall::StaticClass(), SpawnLoc, SpawnInfo);
		walls.Add(CurrentFlora);
	}

	for (float i = -maxCoordX; i < maxCoordX; i += 600)
	{
		FTransform SpawnLoc;
		SpawnLoc.SetLocation(FVector(i, maxCoordY - this->wallsShift, 0));
		SpawnLoc.SetRotation(FQuat(FRotator(0, 180, 0)));
		SpawnLoc.SetScale3D(FVector(1, 1, 1));
		AActor* CurrentFlora = GetWorld()->SpawnActor<AActor>(AWall::StaticClass(), SpawnLoc, SpawnInfo);
		walls.Add(CurrentFlora);
	}

	for (float i = -maxCoordY; i < maxCoordY; i += 600)
	{
		FTransform SpawnLoc;
		SpawnLoc.SetLocation(FVector(-maxCoordX + this->wallsShift, i, 0));
		SpawnLoc.SetRotation(FQuat(FRotator(0, 270, 0)));
		SpawnLoc.SetScale3D(FVector(1, 1, 1));
		AActor* CurrentFlora = GetWorld()->SpawnActor<AActor>(AWall::StaticClass(), SpawnLoc, SpawnInfo);
		walls.Add(CurrentFlora);
	}

	for (float i = -maxCoordY; i < maxCoordY; i += 600)
	{
		FTransform SpawnLoc;
		SpawnLoc.SetLocation(FVector(maxCoordX - this->wallsShift, i, 0));
		SpawnLoc.SetRotation(FQuat(FRotator(0, 90, 0)));
		SpawnLoc.SetScale3D(FVector(1, 1, 1));
		AActor* CurrentFlora = GetWorld()->SpawnActor<AActor>(AWall::StaticClass(), SpawnLoc, SpawnInfo);
		walls.Add(CurrentFlora);
	}



	
	/*FRotator RotationWall1(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnInfoWall1;
	SpawnInfoWall1.Owner = this;

	//auto wall1 = GetWorld()->SpawnActor<AWall>( , &LocationWall1, &RotationWall1, SpawnInfoWall1);
	//wall1->SetActorScale3D(FVector(1, 1000 * h_area, 10));
	//wall1->SetActorScale3D(FVector(1, 1, 1));
	

	FVector LocationWall2(0.0f, BoxExtent.Y + 50, 0.0f);
	FRotator RotationWall2(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnInfoWall2;
	SpawnInfoWall2.Owner = this;

	auto wall2 = GetWorld()->SpawnActor<AWall>(LocationWall2, RotationWall2, SpawnInfoWall2);
	//wall2->SetActorScale3D(FVector(1000 * w_area, 1, 10));
	wall2->SetActorScale3D(FVector(1, 1, 1));

	FVector LocationWall3(-BoxExtent.X - 50, 0.0f, 0.0f);
	FRotator RotationWall3(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnInfoWall3;
	SpawnInfoWall3.Owner = this;

	auto wall3 = GetWorld()->SpawnActor<AWall>(LocationWall3, RotationWall3, SpawnInfoWall3);
	//wall3->SetActorScale3D(FVector(1, 1000 * h_area, 10));
	wall3->SetActorScale3D(FVector(1, 1, 1));

	FVector LocationWall4(0.0f, -BoxExtent.Y - 50, 0.0f);
	FRotator RotationWall4(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnInfoWall4;
	SpawnInfoWall4.Owner = this;

	auto wall4 = GetWorld()->SpawnActor<AWall>(LocationWall4, RotationWall4, SpawnInfoWall4);
	//wall4->SetActorScale3D(FVector(1000 * w_area, 1, 10));
	wall4->SetActorScale3D(FVector(1, 1, 1));*/
}

void AAICarGameMode::createCamera(int scale)
{
	FVector Location(0, 0, scale * additionalCoefHeightOfCamera);
	FRotator Rotation(-45, 90, 0.0f);
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = this;
	auto newCamera = GetWorld()->SpawnActor<AMyCamera>(Location, Rotation, SpawnInfo);
	newCamera->Rename(*FString("Camera_main"));
	saveStringSetting("car", "nameMainCamera", newCamera->GetName());
	

	FString nameCamera = newCamera->GetDebugName(newCamera);
	//DEBUGMESSAGE("Camera: %s or %s", *nameCamera, *newCamera->GetName());
	// ������������ � ���� ����� ������
	if (cameraMainNeed)
	{
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		PlayerController->SetViewTarget(newCamera);
	}
	//auto mainCar = FindActorByName(GetWorld(), FString("TP_VehiclePawn_9"));// = newCamera->GetName();
}

void AAICarGameMode::createMainTargetActor()
{
	FVector LocationFloor(FMath::FRandRange(-maxCoordX + padding*MM, maxCoordX - padding*MM), FMath::FRandRange(-maxCoordY + padding*MM, maxCoordY - padding*MM), 0.0f);
	FRotator RotationFloor(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnInfoFloor;
	SpawnInfoFloor.Owner = this;
	UE_LOG(LogTemp, Warning, TEXT("createMainTargetActor : maxCoordX=%d padding=%d"), maxCoordX, padding*MM);
	mainTargetActor = GetWorld()->SpawnActor<AMainTargetActor>(LocationFloor, RotationFloor, SpawnInfoFloor);
}

void AAICarGameMode::createTextActor()
{
	// �������� ��������� �� Actor
	//AMyText* text;
	FVector SpawnLocation(0.f, 0.f, 0.f); // ���������� �������� �������������� ��� ������ ������
	FActorSpawnParameters SpawnParams;
	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(AMyText::StaticClass(), SpawnLocation, FRotator::ZeroRotator, SpawnParams);

	if (SpawnedActor)
	{
		AMyText* TextActor = Cast<AMyText>(SpawnedActor);
		if (TextActor)
		{
			FString Text = TEXT("������, ���!"); // ���������� �������� �����
			TextActor->SetText(Text);
		}
	}
}

void AAICarGameMode::saveStringSetting(FString section, FString param, FString value)
{
	GConfig->SetString(*section, *param, *value, GGameIni);
}

// ������� ��� ������ ������ �� �����
AActor* AAICarGameMode::FindActorByName(UWorld* World, FString ActorName)
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

void AAICarGameMode::BeginPlay()
{
	//createTextActor();
	FVector x(1, 0, 0);
	FVector y(0, 1, 0);
	FVector z(0, 0, 1);

	createFloorAndWall();
	
	createMainTargetActor();

	int scale = (89013 * h_area + 198.72); //���������� ����� - ����������� ��������� ���������

	createCamera(scale);

	auto mainCar = FindActorByName(GetWorld(), FString("TP_VehiclePawn_9"));
	auto mainCarCast = Cast<AAICarPawn>(mainCar);
	if (this->staticPosition) {
		FVector Location(0.0f, 0.0f, 0.0f);
		FRotator Rotation(0.0f, 0.0f, 0.0f);
		bool res = mainCar->SetActorLocation(Location, true, nullptr, ETeleportType::ResetPhysics);
		if (res) {
			FVector Locatio1n(0.0f, 0.0f, 0.0f);
			mainCar->SetActorLocation(Location, true, nullptr, ETeleportType::ResetPhysics);
		}
	}
	
	if (!this->binded) {
		// Bind function delegate
		mainCarCast->OnCalculateCustomPhysics.BindUObject(this, &AAICarGameMode::CustomPhysics);
		this->binded = true;
	}

	FActorSpawnParameters MySpawnInfo;
	MySpawnInfo.Owner = this;
	auto MyNewTarget = GetWorld()->SpawnActor<ATargetActor>(mainTargetActor->GetActorLocation(), mainTargetActor->GetActorRotation(), MySpawnInfo);

	mainCarCast->targetActor = MyNewTarget;

	FCarInfo MyCarInfo;
	MyCarInfo.centerOfMass = mainCarCast->GetMesh()->GetCenterOfMass();
	MyCarInfo.carLength = float(455.0 / M);
	MyCarInfo.carWidth = float(170.0 / M);
	MyCarInfo.interwheelDistance = float(150.0 / M);
	MyCarInfo.frontToBackWheelDistance = float(270.0 / M);
	MyCarInfo.myNumber = myCars.Num();
	MyCarInfo.location = FVector(mainCarCast->GetActorLocation() / M);
	MyCarInfo.rotation = mainCarCast->GetActorRotation();
	MyCarInfo.cars = mainCarCast;

	// ������� ���������� ��� ������� � ����������� ���
	AMyPlayerController* MyCarController = GetWorld()->SpawnActor<AMyPlayerController>();
	if (MyCarController)
	{
		//CarController->Possess(mainCarCast);
		AMyPlayerState* MyCustomPlayerState = GetWorld()->SpawnActor<AMyPlayerState>();
		mainCarCast->SetPlayerState(MyCustomPlayerState);
		MyCustomPlayerState->CarHealth = 100.0f; // ���������� ��������� ��������� �������

		MyCarController->setCar(mainCarCast);
		MyCarController->setId(myCars.Num());
		MyCarController->setMaxCarCount(myCars.Num());
		myCarsController.Add(MyCarController);
	}

	targets.Add(MyNewTarget);
	carInfoArr.Add(MyCarInfo);
	myCars.Add(mainCarCast);

	for (int i = 0; i < maxCarCount; i++)
	{
		FVector Location(FMath::FRandRange(-maxCoordX + padding*MM, maxCoordX - padding*MM), FMath::FRandRange(-maxCoordY + padding*MM, maxCoordY - padding*MM), 200.0f);
		FRotator Rotation(0, FMath::FRandRange(0, 360), 0.0f);
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Owner = this;

		auto newCar = GetWorld()->SpawnActor<AAICarPawn>(Location, Rotation, SpawnInfo);
		if (newCar)
		{
			auto color = this->ai_env.get_color(i);
			if (color) {
				newCar->GetMesh()->SetMaterial(2, color);
			}
			//DEBUGMESSAGE("Spawned car %d, name %s", i, *newCar->GetName());

			// ������� ���������� ��� ������� � ����������� ���
			AMyPlayerController* CarController = GetWorld()->SpawnActor<AMyPlayerController>();
			if (CarController)
			{
				CarController->Possess(newCar);
				AMyPlayerState* MyCustomPlayerState = GetWorld()->SpawnActor<AMyPlayerState>();
				newCar->SetPlayerState(MyCustomPlayerState);
				MyCustomPlayerState->CarHealth = 100.0f; // ���������� ��������� ��������� �������
				CarController->setCar(newCar);
				CarController->setId(i);
				CarController->setMaxCarCount(myCars.Num());
				myCarsController.Add(CarController);
			}
			FCarInfo carInfo;
			carInfo.centerOfMass = newCar->GetMesh()->GetCenterOfMass() / M;
			carInfo.carLength = float(455.0 / M);
			carInfo.carWidth = float(170.0 / M);
			carInfo.interwheelDistance = float(150.0 / M);
			carInfo.frontToBackWheelDistance = float(270.0 / M);
			carInfo.myNumber = i;
			carInfo.location = FVector(Location / M);
			carInfo.rotation = Rotation;
			carInfo.cars = newCar;

			FVector LocationTarget(FMath::FRandRange(-maxCoordX + padding*MM, maxCoordX - padding*MM), FMath::FRandRange(-maxCoordY + padding*MM, maxCoordY - padding*MM), 0.0f);
			FRotator RotationTarget(0.0f, 0.0f, 0.0f);
			auto newTarget = GetWorld()->SpawnActor<ATargetActor>(LocationTarget, RotationTarget, SpawnInfo);

			this->targets.Add(newTarget);
			newCar->targetActor = newTarget;

			newCar->maxX_AreaOfSpawn = w_area * MM / 2;
			newCar->maxY_AreaOfSpawn = h_area * MM / 2;

			carInfoArr.Add(carInfo);
			myCars.Add(newCar);
		}
	}

	this->ai_env.init(TEXT("ws://127.0.0.1:5055/ws/"), this->myCarsController);
}

/*void AAICarGameMode::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
	//Super::SetupPlayerInputComponent(InputComponent);
	PrimaryActorTick.bCanEverTick = true;
	// set up gameplay key bindings
	check(InputComponent);
	InputComponent->BindAction("SwitchCameraMain", IE_Pressed, this, &AAICarGameMode::OnToggleCamera);
}*/

void AAICarGameMode::OnToggleCamera()
{
	UE_LOG(LogTemp, Warning, TEXT("OnToggleCamera"))
}

void AAICarGameMode::OnInputKeyDown(FKey Key, EInputEvent Event)
{
	// ���� ������ ������ F
	if (Key == EKeys::F)
	{
		// �������� ��������� �� ���������� ������
		//AActor* PreviousCamera = GetWorld()->GetFirstPlayerController()->GetPreviousViewTarget();

		// ������������ � ���� ���������� ������
		//PlayerController->SetViewTarget(PreviousCamera);
	}
}

void AAICarGameMode::loadToEnv(bool show = false)
{
	//env.objects.clear();
	env.targets.clear();
	env.params.num_cars = myCars.Num();
	Vec size = Vec(this->env.X2 / 2, this->env.Y2 / 2);
	//if (show) { UE_LOG(LogTemp, Warning, TEXT("size=%f:%f"), size.x, size.y); }

	if (env.objects.size() == 0) {
		for (int i = 0; i < carInfoArr.Num(); i++)
		{
			double angleCar = (double)carInfoArr[i].rotation.Yaw;

			Vec loc = Vec(carInfoArr[i].location.X + size.x, carInfoArr[i].location.Y + size.y);
			Vec vel = Vec(carInfoArr[i].physicsLinearVelocity.X, carInfoArr[i].physicsLinearVelocity.Y); // ����-����������� ����. ����� - ������� ������ ?
			Vec dir = Vec().fromAngle(angleCar / 180 * M_PI, 1.); // ��������, ����������� ������

			Car _car(loc, vel, (int)carInfoArr[i].carWidth, (int)carInfoArr[i].carLength, (int)carInfoArr[i].carLength,
				env.params.car.vel_max, env.params.car.acc_max, dir, 1.);
			env.objects.push_back(_car);

			Vec target = Vec(this->targets[i]->GetActorLocation().X + size.x, this->targets[i]->GetActorLocation().Y + size.y);
			env.targets.push_back(target);

			if (false) {
				UE_LOG(LogTemp, Warning, TEXT("Env[%i] loc=%i,%i target=%i,%i vel=%f,%f len=%i angle=%f dir=%f,%f angle=%f"),
					i, int(loc.x), int(loc.y), int(target.x), int(target.y),
					vel.x, vel.y, int(vel.length()), vectorToAngle(vel), dir.x, dir.y, angleCar);
			}
		}
	}
	else {
		for (int i = 0; i < carInfoArr.Num(); i++)
		{
			double angleCar = (double)carInfoArr[i].rotation.Yaw;

			Vec loc = Vec(carInfoArr[i].location.X + size.x, carInfoArr[i].location.Y + size.y);
			Vec vel = Vec(carInfoArr[i].physicsLinearVelocity.X, carInfoArr[i].physicsLinearVelocity.Y); // ����-����������� ����. ����� - ������� ������ ?
			Vec dir = Vec().fromAngle(angleCar / 180 * M_PI, 1.); // ��������, ����������� ������

			//Car _car(loc, vel, (int)carInfoArr[i].carWidth, (int)carInfoArr[i].carLength, (int)carInfoArr[i].carLength,
			//	env.params.car.vel_max, env.params.car.acc_max, dir, 1.);
			env.objects[i].init(loc, vel, dir);

			Vec target = Vec(this->targets[i]->GetActorLocation().X + size.x, this->targets[i]->GetActorLocation().Y + size.y);
			env.targets.push_back(target);

			if (false) {
				UE_LOG(LogTemp, Warning, TEXT("Env[%i] loc=%i,%i target=%i,%i vel=%f,%f len=%i angle=%f dir=%f,%f angle=%f"),
					i, int(loc.x), int(loc.y), int(target.x), int(target.y),
					vel.x, vel.y, int(vel.length()), vectorToAngle(vel), dir.x, dir.y, angleCar);
			}
		}
	}
	
}

double AAICarGameMode::vectorToAngle(FVector inputVector)
{
	// ������ ���� � ��������
	double angle = atan2(inputVector.Y, inputVector.X);

	// �������������� ���� � �������
	angle = angle * 180 / M_PI;
	return angle;
} 

double AAICarGameMode::vectorToLenght(FVector inputVector) {

	// ������ ����� �������
	double length = sqrt(inputVector.X * inputVector.X + inputVector.Y * inputVector.Y);

	return length;
}

double AAICarGameMode::vectorToAngle(Vec inputVector)
{
	// ������ ���� � ��������
	double angle = atan2(inputVector.y, inputVector.x);

	// �������������� ���� � �������
	angle = angle * 180 / M_PI;
	return angle;
}

double AAICarGameMode::vectorToLenght(Vec inputVector) {

	// ������ ����� �������
	double length = sqrt(inputVector.x * inputVector.x + inputVector.y * inputVector.y);

	return length;
}

// ������� ��� ���������� ���������� ������������ ���� ��������
float AAICarGameMode::DotProduct(const FVector& v1, const FVector& v2)
{
	return v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z;
}

// ������� ��� ���������� ����� �������
float AAICarGameMode::Magnitude(const FVector& v)
{
	return sqrt(v.X * v.X + v.Y * v.Y + v.Z * v.Z);
}

float AAICarGameMode::AngleBetweenVectors(const FVector& v1, const FVector& v2)
{
	float dotProduct = DotProduct(v1, v2);
	float magnitude1 = Magnitude(v1);
	float magnitude2 = Magnitude(v2);

	// ������ �� ������� �� ����
	if (magnitude1 == 0.0f || magnitude2 == 0.0f)
	{
		return 0.0f;
	}

	// ���������� ���� � ��������
	float cosTheta = dotProduct / (magnitude1 * magnitude2);
	float theta = acos(cosTheta);

	// �������������� ���� � �������
	float angleInDegrees = theta * 180.0f / 3.14159265f;

	return angleInDegrees;
}

FVector AAICarGameMode::createNewTarget(int num)
{
	//targets[num] = FVector(0, 0, 0);
	return FVector(0, 0, 0);
}

void AAICarGameMode::FillCarInfo()
{
	for (auto i = 0; i < myCars.Num(); i++)
	{
		carInfoArr[i].centerOfMass = FVector(myCars[i]->GetMesh()->GetCenterOfMass() / M);
		carInfoArr[i].physicsLinearVelocity = FVector(myCars[i]->GetMesh()->GetPhysicsLinearVelocity() / M);
		carInfoArr[i].physicsAngularVelocityInDegrees = myCars[i]->GetMesh()->GetPhysicsAngularVelocityInDegrees();
        carInfoArr[i].rotation.Yaw = myCars[i]->GetActorRotation().Yaw;
		carInfoArr[i].location = FVector(myCars[i]->GetActorLocation() / M);
		carInfoArr[i].targetPosition = FVector(myCars[i]->targetActor->GetActorLocation() / M);
		carInfoArr[i].targetVelocity = FVector(0, 0, 0);
		//myCars[i]->GetVehicleMovement();
		//carInfoArr[i].speed = FMath::FloorToInt(FMath::Abs(myCars[i]->GetVehicleMovement() ->GetVehicleMovement()->GetForwardSpeed()) * 0.036f);
	
		// wheels stats
		UWheeledVehicleMovementComponent4W* vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(myCars[i]->GetVehicleMovement());
		carInfoArr[i].wheelsSteerAngle = { vehicle4W->Wheels[0]->GetSteerAngle() * M_PI / 180.0, vehicle4W->Wheels[1]->GetSteerAngle() * M_PI / 180.0 }; // TODO
	}
}

void AAICarGameMode::saveFloatSetting(FString section, FString param, float value)
{
	GConfig->SetFloat(*section, *param, value, GGameIni);
}

FString AAICarGameMode::readStringSetting(FString section, FString param)
{
	FString temp;
	if (GConfig->GetString(*section, *param, temp, GGameIni))
	{
		UE_LOG(LogTemp, Warning, TEXT("read from ini : %s"), *temp);
	}
	return temp;
}

float AAICarGameMode::readFloatSetting(FString section, FString param)
{
	float value;
	if (GConfig->GetFloat(*section, *param, value, GGameIni))
	{
		UE_LOG(LogTemp, Warning, TEXT("read from ini : %d"), value);
	}
	return value;
}

bool AAICarGameMode::readBoolSetting(FString section, FString param)
{
	bool value;
	if (GConfig->GetBool(*section, *param, value, GGameIni))
	{
		UE_LOG(LogTemp, Warning, TEXT("read from ini : %d"), value);
	}
	return value;
}

void AAICarGameMode::setCarInfo(FCarInfo inputCarInfo)
{
	//this->carInfo = inputCarInfo;
	bInputCarInfo = true;
}

void AAICarGameMode::UpdateCarState()
{

}

void AAICarGameMode::SlowTick(bool show=false)
{
	for (int i = 0; i < myCars.Num(); i++)
	{
		if (targets[i]->GetActorLocation() != myCars[i]->targetActor->GetActorLocation())
		{
			targets[i] = myCars[i]->targetActor;
			env.targets[i] = Vec(myCars[i]->targetActor->GetActorLocation().X, myCars[i]->targetActor->GetActorLocation().Y);
			DEBUGMESSAGE("+++++++");
		}
	}
	FillCarInfo();
	saveCarInfoToJson("carInfo.json", true);
    loadToEnv(show);
	std::vector<JSON> vecArr = env.step();
	for (int i = 0; i < vecArr.size(); i++)
	{		
        auto log = vecArr[i].log;
		//DEBUGMESSAGE("lenght %f", lenght);
		if (show) {
			//UE_LOG(LogTemp, Warning, TEXT("vecArr[%i] force=%f,%f force3=%f,%f a=%f vel=%f,%f a=%f dir=%f,%f"),
			//	i, vecArr[i].force.x, vecArr[i].force.y, log.force3.x, log.force3.y, log.force3.angle() * 180 / M_PI,
			//	env.objects[i].vel.x, env.objects[i].vel.y, env.objects[i].vel.angle(), env.objects[i].dir.x, env.objects[i].dir.y);
			//UE_LOG(LogTemp, Warning, TEXT("vecArr[%i] pos=%f,%f lastPos=%f,%f force3=%f,%f"),
			//	i, log.force1.x, log.force1.y, log.force2.x, log.force2.y, log.force3.x, log.force3.y);
            //UE_LOG(LogTemp, Warning, TEXT("vecArr[%i] gas=%f rot=%f, newAngleLimit=%f,"),
			//	i, vecArr[i].gas, vecArr[i].rotate, log.newAngleLimit * 180 / M_PI);

        }
		myCarsController[i]->setForward(vecArr[i].gas);
		myCarsController[i]->setRight(vecArr[i].rotate);
	}

	for (int i = 0; i < myCars.Num(); i++)
	{
		auto rot = myCars[i]->GetActorRotation();
		if (rot.Pitch > 70 || rot.Pitch < -70 || rot.Roll > 70 || rot.Roll < -70)
		{
			//myCars[i]->Destroy();
		}
	}

	//for (int i = 0; i < myCars.Num(); i++)
	//{
	//	bool state = myCars[i]->getStatePause();
	//	myCarsController[i]->pause(state);
	//}

	//DEBUGMESSAGE("SlowTick");
}

void AAICarGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UE_LOG(LogTemp, Warning, TEXT("Tick %f"), DeltaSeconds);
	//FillCarInfo();
	FString JSONPayload = this->saveCarInfoToJson("carInfo.json", true, this->ai_env.reset);
	//this->ai_env.tick(DeltaSeconds, JSONPayload);
	UE_LOG(LogTemp, Warning, TEXT("Tick end"));
}


// Called every substep for selected body instance
void AAICarGameMode::CustomPhysics(float DeltaSeconds, FBodyInstance* BodyInstance)
{
	UE_LOG(LogTemp, Warning, TEXT("CustomPhysics::tick %f"), DeltaSeconds);
	FillCarInfo();
	FString JSONPayload = this->saveCarInfoToJson("carInfo.json", false, this->ai_env.reset);
	this->ai_env.tick(DeltaSeconds, JSONPayload);
	UE_LOG(LogTemp, Warning, TEXT("CustomPhysics::tick end"));
}

void AAICarGameMode::TimerCallback()
{
	UE_LOG(LogTemp, Warning, TEXT("TimerCallback"));
}

TArray<FString> AAICarGameMode::GetActorCornerCoordinates(AActor* Actor)
{
	TArray<FString> CornerCoordinates;

	if (Actor)
	{
		// �������� ���������� �����
		TArray<UStaticMeshComponent*> StaticMeshComponents;
		Actor->GetComponents<UStaticMeshComponent>(StaticMeshComponents);

		// ���������� ��� ���������� ����������� ����� �����
		for (UStaticMeshComponent* StaticMeshComp : StaticMeshComponents)
		{
			// �������� ����������� ����� ����������
			UStaticMesh* StaticMesh = StaticMeshComp->GetStaticMesh();

			if (StaticMesh)
			{
				// �������� ������� ����������� �����
				FBoxSphereBounds Bounds = StaticMesh->GetBoundingBox();

				// �������� ���������� ����� �������
				FVector Min = (Bounds.GetBox().Min + FVector(Actor->GetActorLocation().X, Actor->GetActorLocation().Y, 0))/M;
				FVector Max = (Bounds.GetBox().Max + FVector(Actor->GetActorLocation().X, Actor->GetActorLocation().Y, 0))/M;

				// �������� ������ �����
				float Height = Bounds.GetBox().GetSize().Z;

				// ����������� ���������� � ������ � ������ � ��������� �� � ������
				CornerCoordinates.Add(FString::Printf(TEXT("Top left:%f,%f,%f"), Min.X, Min.Y, Max.Z));
				CornerCoordinates.Add(FString::Printf(TEXT("Top right:%f,%f,%f"), Max.X, Min.Y, Max.Z));
				CornerCoordinates.Add(FString::Printf(TEXT("Bottom left:%f,%f,%f"), Min.X, Max.Y, Max.Z));
				CornerCoordinates.Add(FString::Printf(TEXT("Bottom right:%f,%f,%f"), Max.X, Max.Y, Max.Z));
			}
		}
	}

	return CornerCoordinates;
}

FString AAICarGameMode::saveCarInfoToJson(const FString& FileName, bool isSave, bool useStatic)
{
	FString JsonStr;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonStr);

	JsonWriter->WriteObjectStart();

	// GameState
	JsonWriter->WriteObjectStart("GameState");
	JsonWriter->WriteValue("started", true);
	JsonWriter->WriteValue("file_name", FileName);
	JsonWriter->WriteValue("version", 4.26f);
	JsonWriter->WriteValue("realtimeSeconds", GetWorld()->GetTimeSeconds());
	JsonWriter->WriteValue("time", FDateTime::Now().ToString());
	JsonWriter->WriteValue("timeUTC", FDateTime::UtcNow().ToString());
	JsonWriter->WriteObjectEnd();

	// carInfo
	JsonWriter->WriteObjectStart("carInfo");

	// car
	//JsonWriter->WriteObjectStart("car");
	//JsonWriter->WriteValue("name", TEXT("MainCar"));
	//JsonWriter->WriteArrayStart(TEXT("location"));
	//JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[carInfoArr.Num()-1].location.X:-1);
	//JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[carInfoArr.Num()-1].location.Y :-1);
	//JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[carInfoArr.Num()-1].location.Z :-1);
	//JsonWriter->WriteArrayEnd();
	//JsonWriter->WriteArrayStart(TEXT("rotation"));
	//JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[carInfoArr.Num()-1].rotation.Pitch:-1);
	//JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[carInfoArr.Num()-1].rotation.Roll:-1);
	//JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[carInfoArr.Num()-1].rotation.Yaw:-1);
	//JsonWriter->WriteArrayEnd();
	//JsonWriter->WriteArrayStart(TEXT("physicsLinearVelocity"));
	//JsonWriter->WriteValue((carInfoArr.Num()) ?	carInfoArr[carInfoArr.Num()-1].physicsLinearVelocity.X:-1);
	//JsonWriter->WriteValue((carInfoArr.Num()) ?	carInfoArr[carInfoArr.Num()-1].physicsLinearVelocity.Y:-1);
	//JsonWriter->WriteValue((carInfoArr.Num()) ?	carInfoArr[carInfoArr.Num()-1].physicsLinearVelocity.Z:-1);
	//JsonWriter->WriteArrayEnd();
	//JsonWriter->WriteArrayStart(TEXT("physicsAngularVelocityInDegrees"));
	//JsonWriter->WriteValue((carInfoArr.Num()) ?	carInfoArr[carInfoArr.Num()-1].physicsAngularVelocityInDegrees.X:-1);
	//JsonWriter->WriteValue((carInfoArr.Num()) ?	carInfoArr[carInfoArr.Num()-1].physicsAngularVelocityInDegrees.Y:-1);
	//JsonWriter->WriteValue((carInfoArr.Num()) ?	carInfoArr[carInfoArr.Num()-1].physicsAngularVelocityInDegrees.Z:-1);
	//JsonWriter->WriteArrayEnd();
	//JsonWriter->WriteArrayStart(TEXT("centerOfMass"));
	//JsonWriter->WriteValue((carInfoArr.Num()) ?	carInfoArr[carInfoArr.Num()-1].centerOfMass.X:-1);
	//JsonWriter->WriteValue((carInfoArr.Num()) ?	carInfoArr[carInfoArr.Num()-1].centerOfMass.Y:-1);
	//JsonWriter->WriteValue((carInfoArr.Num()) ?	carInfoArr[carInfoArr.Num()-1].centerOfMass.Z:-1);
	//JsonWriter->WriteArrayEnd();
	//JsonWriter->WriteArrayStart(TEXT("t_pos")); // target position
	//JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[carInfoArr.Num() - 1].targetPosition.X : -1);
	//JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[carInfoArr.Num() - 1].targetPosition.Y : -1);
	//JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[carInfoArr.Num() - 1].targetPosition.Z : -1);
	//JsonWriter->WriteArrayEnd();
	//JsonWriter->WriteArrayStart(TEXT("t_vel")); // target velocity
	//JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[carInfoArr.Num() - 1].targetVelocity.X : -1);
	//JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[carInfoArr.Num() - 1].targetVelocity.Y : -1);
	//JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[carInfoArr.Num() - 1].targetVelocity.Z : -1);
	//JsonWriter->WriteArrayEnd();
	//JsonWriter->WriteValue("myNumber", 0);
	////JsonWriter->WriteValue("speed", 24);
	//JsonWriter->WriteObjectEnd();

	// cars array
	JsonWriter->WriteArrayStart("cars");
	for (int i = 0; i < myCars.Num(); i++)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("name", myCars[i]->GetName());
		JsonWriter->WriteArrayStart(TEXT("location"));
		JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[i].location.X:-1);
		JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[i].location.Y:-1);
		JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[i].location.Z:-1);
		JsonWriter->WriteArrayEnd();
		JsonWriter->WriteArrayStart(TEXT("rotation"));
		JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[i].rotation.Pitch:-1);
		JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[i].rotation.Roll:-1);
		JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[i].rotation.Yaw:-1);
		JsonWriter->WriteArrayEnd();
		JsonWriter->WriteArrayStart(TEXT("physicsLinearVelocity"));
		JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[i].physicsLinearVelocity.X: -1);
		JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[i].physicsLinearVelocity.Y: -1);
		JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[i].physicsLinearVelocity.Z: -1);
		JsonWriter->WriteArrayEnd();
		JsonWriter->WriteArrayStart(TEXT("physicsAngularVelocityInDegrees"));
		JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[i].physicsAngularVelocityInDegrees.X: -1);
		JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[i].physicsAngularVelocityInDegrees.Y: -1);
		JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[i].physicsAngularVelocityInDegrees.Z: -1);
		JsonWriter->WriteArrayEnd();
		JsonWriter->WriteArrayStart(TEXT("centerOfMass"));
		JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[i].centerOfMass.X : -1);
		JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[i].centerOfMass.Y : -1);
		JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[i].centerOfMass.Z : -1);
		JsonWriter->WriteArrayEnd();
		JsonWriter->WriteArrayStart(TEXT("wheels"));
		JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[i].wheelsSteerAngle[0] : 0.0);   // TODO
		JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[i].wheelsSteerAngle[1] : 0.0);   // TODO
		JsonWriter->WriteArrayEnd();
		JsonWriter->WriteArrayStart(TEXT("t_pos")); // target position
		JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[i].targetPosition.X : -1);
		JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[i].targetPosition.Y : -1);
		JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[i].targetPosition.Z : -1);
		JsonWriter->WriteArrayEnd();
		JsonWriter->WriteArrayStart(TEXT("t_vel")); // target velocity
		JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[i].targetVelocity.X : -1);
		JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[i].targetVelocity.Y : -1);
		JsonWriter->WriteValue((carInfoArr.Num()) ? carInfoArr[i].targetVelocity.Z : -1);
		JsonWriter->WriteArrayEnd();
		JsonWriter->WriteValue("myNumber", carInfoArr[i].myNumber);
		//JsonWriter->WriteValue("speed", carInfoArr[i].speed);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	JsonWriter->WriteObjectEnd();

	if (useStatic) {
		JsonWriter->WriteObjectStart(TEXT("StaticObjects"));

		// kinds
		JsonWriter->WriteArrayStart(TEXT("kinds"));
		TArray<int> kinds = this->ai_env.get_kinds(this->carInfoArr.Num());
		for (int i = 0; i < kinds.Num(); i++) {
			JsonWriter->WriteValue(kinds[i]);
		}
		JsonWriter->WriteArrayEnd();

		// carSetting
		JsonWriter->WriteObjectStart(TEXT("carSetting"));
		JsonWriter->WriteValue("car_length", (carInfoArr.Num()) ? carInfoArr[0].carLength : -1);
		JsonWriter->WriteValue("car_width", (carInfoArr.Num()) ? carInfoArr[0].carWidth : -1);
		JsonWriter->WriteValue("interwheel_distance", (carInfoArr.Num()) ? carInfoArr[0].interwheelDistance : 1);
		JsonWriter->WriteValue("front_to_back_wheel_distance", (carInfoArr.Num()) ? carInfoArr[0].frontToBackWheelDistance : -1);
		JsonWriter->WriteObjectEnd();

		// fieldSetting
		JsonWriter->WriteObjectStart(TEXT("fieldSetting"));
		JsonWriter->WriteValue("field_height", FMath::RoundToInt((carInfoArr.Num()) ? h_area * MM / M : -1));
		JsonWriter->WriteValue("field_width", FMath::RoundToInt((carInfoArr.Num()) ? w_area * MM / M : -1));
		JsonWriter->WriteObjectEnd();

		// walls array
		JsonWriter->WriteArrayStart("walls");
		for (int i = 0; i < this->wallsInfo.Num(); i++) {
			auto wall = this->wallsInfo[i];
			JsonWriter->WriteObjectStart();

			JsonWriter->WriteArrayStart(TEXT("pos1"));
			JsonWriter->WriteValue(wall.pos1.X);
			JsonWriter->WriteValue(wall.pos1.Y);
			JsonWriter->WriteValue(wall.pos1.Z);
			JsonWriter->WriteArrayEnd();

			JsonWriter->WriteArrayStart(TEXT("pos2"));
			JsonWriter->WriteValue(wall.pos2.X);
			JsonWriter->WriteValue(wall.pos2.Y);
			JsonWriter->WriteValue(wall.pos2.Z);
			JsonWriter->WriteArrayEnd();

			JsonWriter->WriteValue("height", wall.height);

			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteArrayEnd();

		JsonWriter->WriteObjectEnd();
	}

	//JsonWriter->WriteArrayStart("wallCoord");
	//for (int i = 0; i < walls.Num(); i++)
	//{
	//	JsonWriter->WriteObjectStart();
	//	JsonWriter->WriteValue("name", i);
	//	TArray<FString> CornerCoordinates = GetActorCornerCoordinates(walls[i]);
	//	for (const FString& Coordinate : CornerCoordinates)
	//	{
	//		TArray<FString> SplitStrings;
	//		Coordinate.ParseIntoArray(SplitStrings, TEXT(":"), true);
	//		TArray<FString> SplitStringsCoord;
	//		SplitStrings[1].ParseIntoArray(SplitStringsCoord, TEXT(","), true);
	//
	//		JsonWriter->WriteArrayStart(*SplitStrings[0]);
	//		JsonWriter->WriteValue(*SplitStringsCoord[0]);
	//		JsonWriter->WriteValue(*SplitStringsCoord[1]);
	//		JsonWriter->WriteValue(*SplitStringsCoord[2]);
	//		JsonWriter->WriteArrayEnd();
	//	}
	//	JsonWriter->WriteObjectEnd();
	//}
	//JsonWriter->WriteArrayEnd();


	// Debug
	JsonWriter->WriteObjectStart(TEXT("Debug"));
	JsonWriter->WriteValue("unique_ID", false);
	JsonWriter->WriteValue("debug_level", TEXT("Warning"));
	JsonWriter->WriteValue("ip", TEXT("localhost"));

	JsonWriter->WriteObjectEnd();
	JsonWriter->WriteObjectEnd();
	
	// Close the writer and finalize the output
	JsonWriter->Close();

	if (isSave) {
		FString FullPath = FPaths::ProjectSavedDir();
		FullPath += FileName;
		//UE_LOG(LogTemp, Warning, TEXT("FullPath: %s"), *FullPath);

		FArchive* SaveFile = IFileManager::Get().CreateFileWriter(*FullPath);
		if (SaveFile)
		{
			*SaveFile << JsonStr;
			// this will commit the save to disk and free up your memory :)
			delete SaveFile;
		}
	}
	
	return JsonStr;
}


void AAICarGameMode::destroyGame()
{
	this->ai_env.dispose();
}

void AAICarGameMode::Reset()
{
	DEBUGMESSAGE("Reset");
}

