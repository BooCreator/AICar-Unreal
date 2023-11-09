// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VehicleWheel.h"
#include "AICarWheelRear.generated.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

UCLASS()
class UAICarWheelRear : public UVehicleWheel
{
	GENERATED_BODY()

public:
	UAICarWheelRear();
	float readFloatSetting(FString section, FString param);
};

PRAGMA_ENABLE_DEPRECATION_WARNINGS

