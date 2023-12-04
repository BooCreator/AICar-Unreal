// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "Client/AIEnvironment.h"
#include <string>
#include "GameFramework/GameModeBase.h"
#include "MyPlayerController.h"
#include "TargetActor.h"
#include "Kismet/GameplayStatics.h"
#include <AICar/project/obj/Environment.h>
#include "Components/InputComponent.h"
#include "MainTargetActor.h"
#include <algorithm>
#include "AICarGameMode.generated.h"


struct WallInfo {
public:
	FVector pos1;
	FVector pos2;
	float height;

	WallInfo(float x1, float y1, float x2, float y2, float height) {
		this->pos1 = FVector(x1, y1, 0);
		this->pos2 = FVector(x2, y2, 0);
		this->height = height;
	}
};


USTRUCT(BlueprintType)
struct FCarInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		int32 myNumber = -1;
	UPROPERTY(BlueprintReadWrite)
		APawn* cars = NULL;
	UPROPERTY(BlueprintReadWrite)
		FVector location = FVector(-1, -1, -1);
	UPROPERTY(BlueprintReadWrite)
		FRotator rotation = FRotator(-1, -1, -1);
	UPROPERTY(BlueprintReadWrite)
		FVector centerOfMass = FVector(-1, -1, -1);
	UPROPERTY(BlueprintReadWrite)
		FVector physicsLinearVelocity = FVector(-1, -1, -1);
	UPROPERTY(BlueprintReadWrite)
		FVector physicsAngularVelocityInDegrees = FVector(-1, -1, -1);
	UPROPERTY(BlueprintReadWrite)
		float speed = -1.0;
	UPROPERTY(BlueprintReadWrite)
		int vel_max = 100;
	UPROPERTY(BlueprintReadWrite)
		int acc_max = 200;
	UPROPERTY(BlueprintReadWrite)
		float carLength = -1.0;
	UPROPERTY(BlueprintReadWrite)
		float carWidth = -1.0;
	UPROPERTY(BlueprintReadWrite)
		float realtimeSeconds = -1.0;
	UPROPERTY(BlueprintReadWrite)
		FDateTime time = FDateTime::Now();
	UPROPERTY(BlueprintReadWrite)
		FDateTime timeUTC = FDateTime::UtcNow();
	UPROPERTY(BlueprintReadWrite)
		float  interwheelDistance = -1.0;
	UPROPERTY(BlueprintReadWrite)
		float frontToBackWheelDistance = -1.0;
	UPROPERTY(BlueprintReadWrite)
		FVector targetPosition = FVector(-1, -1, -1);
	UPROPERTY(BlueprintReadWrite)
		FVector targetVelocity = FVector(-1, -1, -1);

	TArray<double> wheelsSteerAngle = { 0.0,0.0 }; // TODO
};

struct Vector3D
{
	float x;
	float y;
	float z;
};

UCLASS(minimalapi)
class AAICarGameMode : public AGameModeBase
{
	GENERATED_BODY()
private:
	AIEnvironment ai_env;
	// Called every substep for selected body instance
	void CustomPhysics(float DeltaSeconds, FBodyInstance* BodyInstance);
	bool binded = false;
	int request_id = 0;
public:
	AAICarGameMode();
	~AAICarGameMode();
	virtual void BeginPlay() override;

	//virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	void OnToggleCamera();

	void loadToEnv(bool show);

	void createCamera(int scale);

	void createMainTargetActor();

	void createTextActor();

	double vectorToAngle(FVector inputVector);

	double vectorToLenght(FVector inputVector);

	double vectorToAngle(Vec inputVector);

	double vectorToLenght(Vec inputVector);

	float DotProduct(const FVector& v1, const FVector& v2);

	float Magnitude(const FVector& v);

	float AngleBetweenVectors(const FVector& v1, const FVector& v2);

	AActor* FindActorByName(UWorld* World, FString ActorName);

	void saveStringSetting(FString section, FString param, FString value);

	FVector createNewTarget(int num);

	void RestoreUpright();

	int maxCarCount = 2;
	double w_area = 1;
	double h_area = 1;
	double padding = 2000;
	double additionalCoefHeightOfCamera = 1;

	const int SCREEN_WIDTH = 100000;
	const int SCREEN_HEIGHT = 100000;

	int X1; int X2; int Y1; int Y2;

	Environment env = Environment(SCREEN_WIDTH, SCREEN_HEIGHT);

	TArray <AAICarPawn*> myCars;
	TArray <AMyPlayerController*> myCarsController;
	TArray<ATargetActor*> targets;
	AMainTargetActor* mainTargetActor;

	bool cameraMainNeed = false;
	void FillCarInfo();
	void loadParams();
	void initGame();
	//void StartPlay();
	void createFloorAndWall();
	void saveFloatSetting(FString section, FString param, float value);
	float readFloatSetting(FString section, FString param);
	bool readBoolSetting(FString section, FString param);
	FString readStringSetting(FString section, FString param);
	FString saveCarInfoToJson(const FString& FileName, bool isSave = false, bool useStatic = false);
	void createTarget(ATargetActor& createTarget);
	void destroyGame();
	TArray<FString> GetActorCornerCoordinates(AActor* Actor);
	TArray<AActor*> walls;
	float wallsShift = 50.;
	TArray<WallInfo> wallsInfo;
	//--------------------------------------------------------------------------
	Params readParams() { return Params(); }
	//--------------------------------------------------------------------------
	double frand(int p = 100) { return float(rand() % p) / p; }

	UPROPERTY(EditAnywhere, Category = "Control C++")
		TArray<FCarInfo> carInfoArr;
	UFUNCTION(BlueprintCallable)
		void setCarInfo(FCarInfo inputCarInfo);

	void UpdateCarState();
	void SlowTick(bool show);
	virtual void Tick(float DeltaSeconds) override;
	void TimerCallback();
	float TimerDelay = 2.0f;
	virtual void Reset() override;

	void OnInputKeyDown(FKey Key, EInputEvent Event);

	FTimerHandle TimerHandle;

private:
	bool bInputCarInfo = false;
	int delayForSlowCounter = 50;
	int currentForSlowCounter = 0;
	int maxCoordX = 0;
	int maxCoordY = 0;
	int staticPosition = 0;
	FString JSONPayload;
};

