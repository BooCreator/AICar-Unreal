// Fill out your copyright notice in the Description page of Project Settings.


#include "Wall.h"
#include "Components/SphereComponent.h"
#include "Components/ShapeComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AWall::AWall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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
    //StaticMeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, -12.0f));
    StaticMeshComp->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
    StaticMeshComp->SetRelativeScale3D(FVector(1, 1, 1));

    //Using Constructor Helpers to set our Static Mesh Comp with a Sphere Shape.
    //static ConstructorHelpers::FObjectFinder<UStaticMesh>SphereMeshAsset(TEXT("StaticMesh'/Game/Geometry/Meshes/1M_Cube.1M_Cube'"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh>SphereMeshAsset(TEXT("StaticMesh'/Game/RoadBlock1/Concrete_Barrier.Concrete_Barrier'"));
    StaticMeshComp->SetStaticMesh(SphereMeshAsset.Object);
    SphereComp->SetSimulatePhysics(false);

}

// Called when the game starts or when spawned
void AWall::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

