// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MainTargetActor.generated.h"

UCLASS()
class AICAR_API AMainTargetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMainTargetActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void RepeatingFunction();

	void MyTimerFinished();

	int RepeatingCallsRemaining = 10;

	bool debug = false;

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

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float Width = -1;
	float Length = -1;
	float Height = -1;

	bool readBoolSetting(FString section, FString param);
	FTimerHandle MemberTimerHandle;

};
