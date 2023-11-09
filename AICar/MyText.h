// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
//#include "Components/TextRenderComponent.h"
#include "Runtime/Engine/Classes/Components/TextRenderComponent.h"
#include "MyText.generated.h"

UCLASS()
class AICAR_API AMyText : public AActor
{
	GENERATED_BODY()
	
public:	

	FString _text = "!";
	// Sets default values for this actor's properties
	AMyText();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text")
		UTextRenderComponent* TextComponent;

	UFUNCTION(BlueprintCallable, Category = "Text")
		void SpawnText(const FString& Text, const FVector& Location, const FRotator& Rotation);
	void SetText(FString input) { _text = input; };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
