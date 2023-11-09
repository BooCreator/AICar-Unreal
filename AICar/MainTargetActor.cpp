// Fill out your copyright notice in the Description page of Project Settings.


#include "MainTargetActor.h"
#include "Components/SphereComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/CapsuleComponent.h"

#define DEBUGMESSAGE(x, ...) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, FString::Printf(TEXT(x), __VA_ARGS__));}

#define M 100

// Sets default values
AMainTargetActor::AMainTargetActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
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
    StaticMeshComp->SetRelativeScale3D(FVector(20, 20, 20));

    StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    StaticMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);

    static ConstructorHelpers::FObjectFinder<UStaticMesh>SphereMeshAsset(TEXT("StaticMesh'/Game/Sign2/SM_signale.SM_signale'"));
    StaticMeshComp->SetStaticMesh(SphereMeshAsset.Object);
    debug = readBoolSetting("car", "debug");

    UStaticMesh* StaticMesh = StaticMeshComp->GetStaticMesh();

    // Get the bounding box of the static mesh
    FBoxSphereBounds Bounds = StaticMesh->GetBoundingBox();

    // Calculate the size of the bounding box
    FVector Size = Bounds.GetBox().GetSize();

    // The size vector contains the width, length, and height of the actor
    Width = Size.X;
    Length = Size.Y;
    Height = Size.Z;
    DEBUGMESSAGE("Spawned main target, name %s, width = %f, length = %f, height = %f", *GetName(), Width, Length, Height);
    //if (debug)
    //{
    //    DEBUGMESSAGE("Spawned main target, name %s", *GetName());
    //}
}

// Called when the game starts or when spawned
void AMainTargetActor::BeginPlay()
{
	Super::BeginPlay();

    FTimerDelegate TimerDelegate;
    TimerDelegate.BindLambda([&]
        {
            UE_LOG(LogTemp, Warning, TEXT("This text will appear in the console 3 seconds after execution"));
        });
    GetWorld()->GetTimerManager().SetTimer(MemberTimerHandle, TimerDelegate, 3, false);

    //GetWorldTimerManager().SetTimer(MemberTimerHandle, this, &AMainTargetActor::RepeatingFunction, 0.10f, true, 2.0f);
    
}

void AMainTargetActor::RepeatingFunction()
{
    // Once we've called this function enough times, clear the Timer.
    if (--RepeatingCallsRemaining <= 0)
    {
        GetWorldTimerManager().ClearTimer(MemberTimerHandle);

        //FTimerDelegate TimerDelegate;
        //TimerDelegate.BindUFunction(this, FName("MyTimerFinished"));
        //GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);
    }
    UE_LOG(LogTemp, Warning, TEXT("RepeatingFunction %f"), GetWorld()->GetTimeSeconds());
    //GetWorldTimerManager().PauseTimer(MemberTimerHandle);
    //GetWorldTimerManager().UnPauseTimer(MemberTimerHandle);
    //GetWorldTimerManager().IsTimerActive(MemberTimerHandle);
    //GetWorldTimerManager().GetTimerRate(MemberTimerHandle);
    //GetWorldTimerManager().GetTimerElapsed(MemberTimerHandle); //возвращая прошедшее время
    //GetWorldTimerManager().GetTimerRemaining(MemberTimerHandle); //возвращая оставшееся время
}

void MyTimerFinished()
{
    UE_LOG(LogTemp, Warning, TEXT("RepeatingFunction1 is finished "));
}

void AMainTargetActor::MyTimerFinished()
{
    UE_LOG(LogTemp, Warning, TEXT("RepeatingFunction2 is finished %f"), GetWorld()->GetTimeSeconds());
}

// Called every frame
void AMainTargetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AMainTargetActor::readBoolSetting(FString section, FString param)
{
    bool value;
    if (GConfig->GetBool(*section, *param, value, GGameIni))
    {
        UE_LOG(LogTemp, Warning, TEXT("read from ini : %d"), value);
    }
    return value;
}

