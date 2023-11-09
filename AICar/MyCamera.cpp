// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCamera.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"


// Sets default values
AMyCamera::AMyCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Set this pawn to be controlled by the lowest-numbered player
    //AutoPossessPlayer = EAutoReceiveInput::Player0;

    // Create a dummy root component we can attach things to.
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    // Create a camera and a visible object
    camera = CreateDefaultSubobject<UCameraComponent>(TEXT("OurCamera"));
    OurVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OurVisibleComponent"));
    // Attach our camera and visible object to our root component. Offset and rotate the camera.
    camera->SetupAttachment(RootComponent);
    camera->SetRelativeLocation(FVector(-250.0f, 0.0f, 250.0f));
    camera->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
    OurVisibleComponent->SetupAttachment(camera);
    OurVisibleComponent->SetRelativeLocation(FVector(268.0f, 0.0f, -14.14f));
    OurVisibleComponent->SetRelativeRotation(FRotator(45.0f, 0.0f, 0.0f));
    RootComponent = camera;
    camera->Activate(true);
    OurVisibleComponent->Activate(true);
}

// Called when the game starts or when spawned
void AMyCamera::BeginPlay()
{
	Super::BeginPlay();
	


    /*this->camera = this->CreateDefaultSubobject<UCameraComponent>(TEXT("CameraС++"));
    this->springArm = this->CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    this->springArm->TargetArmLength = 8000;
    this->SetRootComponent(this->springArm);
    this->camera->SetupAttachment(this->springArm, USpringArmComponent::SocketName);*/

    // Создайте и настройте компонент камеры
    /*CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    if (CameraComponent)
    {
        // Установите параметры камеры, такие как поле зрения, расположение и пр.
        CameraComponent->SetFieldOfView(90.0f);
        CameraComponent->SetRelativeLocation(FVector(0, 0, 0)); // Расположение камеры
        CameraComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
    }*/
}

// Called every frame
void AMyCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

