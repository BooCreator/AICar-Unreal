// Fill out your copyright notice in the Description page of Project Settings.

#include "MyText.h"
#include "Engine/Font.h"
#include "Engine/Engine.h"

// Sets default values
AMyText::AMyText()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    TextComponent = CreateDefaultSubobject<UTextRenderComponent>("TextComponent");
    //TextComponent->SetupAttachment(VisualMesh);
    TextComponent->SetRelativeLocation(FVector(0.0f, -500.0f, 500.0f));
    TextComponent->SetRelativeRotation(FRotator(0.0, 0.0, 0.0));
    TextComponent->SetTextRenderColor(FColor::White);
    TextComponent->SetXScale(1.f);
    TextComponent->SetYScale(1.f);
    TextComponent->SetWorldSize(5000);
    TextComponent->Text = FText::FromString("GameOver ");

}

void AMyText::SpawnText(const FString& Text, const FVector& Location, const FRotator& Rotation)
{
    /*FVector SpawnLocation(0.f, 0.f, 0.f); // Установите желаемое местоположение для спавна текста
    FActorSpawnParameters SpawnParams;
    AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(AMyText::StaticClass(), SpawnLocation, FRotator::ZeroRotator, SpawnParams);

    if (SpawnedActor)
    {
        AMyText* TextActor = Cast<AMyText>(SpawnedActor);
        if (TextActor)
        {
            FString Text = TEXT("Привет, мир!"); // Установите желаемый текст
            TextActor->SetText(Text);
        }
    }*/
}

// Called when the game starts or when spawned
void AMyText::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyText::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

