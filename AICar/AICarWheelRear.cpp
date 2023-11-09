// Copyright Epic Games, Inc. All Rights Reserved.

#include "AICarWheelRear.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

UAICarWheelRear::UAICarWheelRear()
{
	ShapeRadius = 35.f;
	ShapeWidth = 10.0f;
	bAffectedByHandbrake = true;
	SteerAngle = 0.f;
	SuspensionMaxRaise = 10;
	SuspensionMaxDrop = 10;
	SuspensionDampingRatio = readFloatSetting("car", "suspension_damping_ratio");
	LatStiffMaxLoad = readFloatSetting("car", "lat_stiff_max_load");
}

float UAICarWheelRear::readFloatSetting(FString section, FString param)
{
	float value;
	if (GConfig->GetFloat(*section, *param, value, GGameIni))
	{
		UE_LOG(LogTemp, Warning, TEXT("read from ini : %d"), value);
	}
	return value;
}


PRAGMA_ENABLE_DEPRECATION_WARNINGS
