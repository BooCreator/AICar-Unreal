// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Wall.generated.h"

UCLASS()
class AICAR_API AWall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USphereComponent* SphereComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USphereComponent* FloorComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* FloorStaticMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UParticleSystemComponent* ParticleComp;

};
