// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "JsonObjectConverter.h"
#include "CoreMinimal.h"
#include "JsonLoader.generated.h"


USTRUCT()
struct FAction {
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		double gas;
	UPROPERTY()
		double rotate;

	FAction() { this->gas = .0; this->rotate = .0; }
};

USTRUCT()
struct FParams {
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		int fps;
	UPROPERTY()
		int phys_fps;
	UPROPERTY()
		bool speed_up;
	UPROPERTY()
		int steps;

	FParams() { this->fps = 60; this->phys_fps = 100; this->speed_up = false; this->steps = -1; }
};

USTRUCT()
struct FActions {
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		TArray<FAction> cars;
	UPROPERTY()
		FParams params;
	UPROPERTY()
		int request_ID;
	FActions() {}
};
/**
 *
 */
class AICAR_API JsonLoader
{
public:
	JsonLoader();
	~JsonLoader();

	FActions ParseJSON(FString Message);
	FString CreateJSON();
};
