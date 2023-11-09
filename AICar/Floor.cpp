// Fill out your copyright notice in the Description page of Project Settings.


#include "Floor.h"
#include "Components/SphereComponent.h"
#include "Components/ShapeComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"


void AFloor::saveFloatSetting(FString section, FString param, float value)
{
    GConfig->SetFloat(*section, *param, value, GGameIni);
}

float AFloor::readFloatSetting(FString section, FString param)
{
    float value;
    if (GConfig->GetFloat(*section, *param, value, GGameIni))
    {
        UE_LOG(LogTemp, Warning, TEXT("read from ini : %d"), value);
    }
    return value;
}

void AFloor::loadParams()
{
    w_area = readFloatSetting("car", "w_area");
    h_area = readFloatSetting("car", "h_area");
}

// Sets default values
AFloor::AFloor()
{
    loadParams();
    //Creating our Default Components
    SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
    StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    ParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComp"));

    //Attaching the Components and setting physics
    SphereComp->SetupAttachment(RootComponent);
    SphereComp->SetSimulatePhysics(true);
    SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    StaticMeshComp->AttachToComponent(SphereComp, FAttachmentTransformRules::KeepRelativeTransform);
    ParticleComp->AttachToComponent(StaticMeshComp, FAttachmentTransformRules::KeepRelativeTransform);

    //Setting the Sphere radius to be of a smaller size in line with the Static Mesh.
    SphereComp->SetSphereRadius(2000.0f);

    //Setting the Static Mesh Scale and Location to fit the radius of the Sphere.
    StaticMeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, -12.0f));
    StaticMeshComp->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
    StaticMeshComp->SetRelativeScale3D(FVector(25 * w_area, 25 * h_area, 1));

    //Using Constructor Helpers to set our Static Mesh Comp with a Sphere Shape.
    static ConstructorHelpers::FObjectFinder<UStaticMesh>SphereMeshAsset(TEXT("StaticMesh'/Game/Geometry/Meshes/Map_Floor_2.Map_Floor_2'"));
    StaticMeshComp->SetStaticMesh(SphereMeshAsset.Object);
    SphereComp->SetSimulatePhysics(false);
}

// Called when the game starts or when spawned
void AFloor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

