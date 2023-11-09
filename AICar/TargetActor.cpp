// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetActor.h"
#include "Components/SphereComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ATargetActor::ATargetActor()
{
    //Creating our Default Components
    SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("TargetOfCar"));
    StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp2"));

    //Attaching the Components and setting physics
    SphereComp->SetupAttachment(RootComponent);
    SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SphereComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

    StaticMeshComp->AttachToComponent(SphereComp, FAttachmentTransformRules::KeepRelativeTransform);

    StaticMeshComp->SetRelativeLocation(FVector(0.0, 0.0, -10.0f));
    StaticMeshComp->SetRelativeRotation(FRotator(0.0, 0.0, 0.0f));
    StaticMeshComp->SetRelativeScale3D(FVector(1.3, 1.3, 1.3));

    StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    StaticMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);


    static ConstructorHelpers::FObjectFinder<UStaticMesh>SphereMeshAsset(TEXT("StaticMesh'/Game/roadcone/roadcone.roadcone'"));
    StaticMeshComp->SetStaticMesh(SphereMeshAsset.Object);

    debug = readBoolSetting("car", "debug");
    if (debug == true)
    {
       
    }
}

// Called when the game starts or when spawned
void ATargetActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATargetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ATargetActor::readBoolSetting(FString section, FString param)
{
    bool value;
    if (GConfig->GetBool(*section, *param, value, GGameIni))
    {
        UE_LOG(LogTemp, Warning, TEXT("read from ini : %d"), value);
    }
    return value;
}
