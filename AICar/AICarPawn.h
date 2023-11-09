
#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "MyCamera.h"
#include "TargetActor.h"
#include "MainTargetActor.h"
#include <time.h>
#include <vector>
#include "Sound/SoundCue.h"
#include "AICarPawn.generated.h"


class UCameraComponent;
class USpringArmComponent;
class UTextRenderComponent;
class UInputComponent;

PRAGMA_DISABLE_DEPRECATION_WARNINGS

UCLASS(config="Game1.ini")
class AAICarPawn : public AWheeledVehicle
{
	GENERATED_BODY()

	/** Spring arm that will offset the camera */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	/** Camera component that will be our viewpoint */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	/** SCene component for the In-Car view origin */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* InternalCameraBase;

	/** Camera component for the In-Car view */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* InternalCamera;

	/** Text component for the In-Car speed */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent* InCarSpeed;

	/** Text component for the In-Car gear */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent* InCarGear;

	
public:
	ATargetActor* targetActor;

	int countOfColumn = 0;

	int countOfMainColumn = 0;

	void saveFloatSetting(FString section, FString param, float value);

	float readFloatSetting(FString section, FString param);

	void saveIntSetting(FString section, FString param, int value);

	int readIntSetting(FString section, FString param);

	void saveStringSetting(FString section, FString param, FString value);

	FString readStringSetting(FString section, FString param);

	void saveBoolSetting(FString section, FString param, bool value);

	bool readBoolSetting(FString section, FString param);

	AAICarPawn();

	float w_area = 1;
	float h_area = 1;
	int debug = 0;
	float padding = 0;
	bool cameraMainNeed = false;
	float minDistanceToTarget = 1000;

	bool needGameOver = false;

	USoundCue* propellerAudioCue;
	UAudioComponent* propellerAudioComponent;

	/** The current speed as a string eg 10 km/h */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
	FText SpeedDisplayString;

	/** The current gear as a string (R,N, 1,2 etc) */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
	FText GearDisplayString;

	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
	/** The color of the incar gear text in forward gears */
	FColor	GearDisplayColor;

	/** The color of the incar gear text when in reverse */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
	FColor	GearDisplayReverseColor;

	/** Are we using incar camera */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly)
	bool bInCarCameraActive;

	/** Are we in reverse gear */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly)
	bool bInReverseGear;

	UPROPERTY(EditAnywhere, Category = "sound")
		class USoundBase* PicketSound;

	double start = FPlatformTime::Seconds();
	double end = FPlatformTime::Seconds();

	double beginCatchTargetTime = 0;
	double endCatchTargetTime = 0;

	std::vector<double> arrTime;
	std::vector<FVector> historyLocation;

	void toogleAutopilot();
	bool autopilot = true;

	double startTimer();
	double stopTimer();

	double distanceOfCar = 0;
	int counter_column_current = 0;
	int counter_column_max = 5;
	/** Initial offset of incar camera */
	FVector InternalCameraOrigin;
	// Begin Pawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	void eventPause();
	bool getStatePause();

	// End Pawn interface
	void createINI();
	void loadColumnCounter();
	void SlowTick(bool isFinish);
	int currentCameraNum=0;
	FString  nameOfCamera = "MyCamera_0";
	FString locationOfInternalCamera = "X=0,Y=-12,Z=10";
	FString rotationOfInternalCamera = "P=0,Y=-12,R=10";
	void setNameOfCamera(FString input);
	// Begin Actor interface
	virtual void Tick(float Delta) override;
	void RestartGame();
	void ExitGame();
protected:
	AActor* FindActorByName(UWorld* World, FString ActorName);
	virtual void BeginPlay() override;
	double currentForSlowCounter = 0;
	double delayForSlowCounter = 100;
	double maxSizeHistory = 10000;

public:
	// End Actor interface

	/** Handle pressing forwards */
	void MoveForward(float Val);

	void MoveForwardManual(float Val);

	/** Setup the strings used on the hud */
	void SetupInCarHUD();

	void RestoreUpright();

	bool playSound();

	/** Update the physics material used by the vehicle mesh */
	void UpdatePhysicsMaterial();
	/** Handle pressing right */
	void MoveRight(float Val);

	void MoveRightManual(float Val);

	void MoveWithVector(FVector Velocity);
	/** Handle handbrake pressed */
	void OnHandbrakePressed();
	/** Handle handbrake released */
	void OnHandbrakeReleased();
	/** Switch between cameras */
	void OnToggleCamera();
	/** Handle reset VR device */
	void OnResetVR();

	static const FName LookUpBinding;
	static const FName LookRightBinding;

	double maxX_AreaOfSpawn = 1000;
	double maxY_AreaOfSpawn = 1000;


private:
	AActor* prevView;
	void EnableIncarView( const bool bForce = false );

	double distancePoints(int x1, int y1, int x2, int y2);

	void trimCoord(double& x, double& y, double r);

	FVector nextCoord(FVector prevPos);

	/** Update the gear and speed strings */
	void UpdateHUDStrings();

	/* Are we on a 'slippery' surface */
	bool bIsLowFriction;
	bool bIsPaused = false;

	/**/
	FBodyInstance* MainBodyInstance;
public:
	FCalculateCustomPhysics OnCalculateCustomPhysics;

public:
	/** Returns SpringArm subobject **/
	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }
	/** Returns Camera subobject **/
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
	/** Returns InternalCamera subobject **/
	FORCEINLINE UCameraComponent* GetInternalCamera() const { return InternalCamera; }
	/** Returns InCarSpeed subobject **/
	FORCEINLINE UTextRenderComponent* GetInCarSpeed() const { return InCarSpeed; }
	/** Returns InCarGear subobject **/
	FORCEINLINE UTextRenderComponent* GetInCarGear() const { return InCarGear; }
};

PRAGMA_ENABLE_DEPRECATION_WARNINGS

