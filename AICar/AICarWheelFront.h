// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VehicleWheel.h"
#include "AICarWheelFront.generated.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

UCLASS()
class UAICarWheelFront : public UVehicleWheel
{
	GENERATED_BODY()

public:
	UAICarWheelFront();
	float readFloatSetting(FString section, FString param);
};

PRAGMA_ENABLE_DEPRECATION_WARNINGS


