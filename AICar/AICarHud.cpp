// Copyright Epic Games, Inc. All Rights Reserved.

#include "AICarHud.h"
#include "AICarPawn.h"
#include "WheeledVehicle.h"
#include "RenderResource.h"
#include "Shader.h"
#include "Engine/Canvas.h"
#include "WheeledVehicleMovementComponent.h"
#include "Engine/Font.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"
#include <numeric>
#include <vector>

#define LOCTEXT_NAMESPACE "VehicleHUD"

#ifndef HMD_MODULE_INCLUDED
#define HMD_MODULE_INCLUDED 0
#endif

AAICarHud::AAICarHud()
{
	static ConstructorHelpers::FObjectFinder<UFont> Font(TEXT("/Engine/EngineFonts/RobotoDistanceField"));
	HUDFont = Font.Object;
}

void AAICarHud::DrawHUD()
{
	Super::DrawHUD();

	// Calculate ratio from 720p
	const float HUDXRatio = Canvas->SizeX / 1280.f;
	const float HUDYRatio = Canvas->SizeY / 720.f;

	bool bWantHUD = true;
#if HMD_MODULE_INCLUDED
	bWantHUD = !GEngine->IsStereoscopic3D();
#endif // HMD_MODULE_INCLUDED
	// We dont want the onscreen hud when using a HMD device	
	if (bWantHUD == true)
	{
		// Get our vehicle so we can check if we are in car. If we are we don't want onscreen HUD
		AAICarPawn* Vehicle = Cast<AAICarPawn>(GetOwningPawn());
		if ((Vehicle != nullptr) && (Vehicle->bInCarCameraActive == false))
		{
			FVector2D ScaleVec(HUDYRatio * 1.4f, HUDYRatio * 1.4f);

			// Counter
			//auto text = Format("%s", FText::FromString(*FString::FromInt(Vehicle->countOfMainColumn)));
			FString resultText = FString::Printf(TEXT("Points: %i/%i"), Vehicle->counter_column_current, Vehicle->counter_column_max);
			
			FCanvasTextItem CounterTextItem(FVector2D(HUDXRatio * 5.f, HUDYRatio * 410), FText::FromString(*resultText), HUDFont, FLinearColor::White);
			CounterTextItem.Scale = ScaleVec;
			Canvas->DrawItem(CounterTextItem);

            if(!Vehicle->endCatchTargetTime)
				resultText = FString::Printf(TEXT("Time: %i sec"), int(Vehicle->GetLastRenderTime()));
			else
				resultText = FString::Printf(TEXT("Time: %i sec"), int(Vehicle->endCatchTargetTime));

			FCanvasTextItem timeItem(FVector2D(HUDXRatio * 5.f, HUDYRatio * 455), FText::FromString(*resultText), HUDFont, FLinearColor::White);
			timeItem.Scale = ScaleVec;
			Canvas->DrawItem(timeItem);

			resultText = FString::Printf(TEXT("Distance: %s m"), (*FString::FromInt(Vehicle->distanceOfCar / 100)));
			FCanvasTextItem distanceOfCarTextItem(FVector2D(HUDXRatio * 5.f, HUDYRatio * 500.f), FText::FromString(*resultText), HUDFont, FLinearColor::White);
			distanceOfCarTextItem.Scale = ScaleVec;
			Canvas->DrawItem(distanceOfCarTextItem);

			// Speed
			resultText = (TEXT("Speed: ") + Vehicle->SpeedDisplayString.ToString());
			FCanvasTextItem SpeedTextItem(FVector2D(HUDXRatio * 5.f, HUDYRatio * 545), FText::FromString(*resultText), HUDFont, FLinearColor::White);
			SpeedTextItem.Scale = ScaleVec;
			Canvas->DrawItem(SpeedTextItem);

			// Gear
			FCanvasTextItem GearTextItem(FVector2D(HUDXRatio * 5.f, HUDYRatio * 590.f), Vehicle->GearDisplayString, HUDFont, Vehicle->bInReverseGear == false ? Vehicle->GearDisplayColor : Vehicle->GearDisplayReverseColor);
			GearTextItem.Scale = ScaleVec;
			Canvas->DrawItem(GearTextItem);


			if (Vehicle->needGameOver)
			{
				// GameOver
				resultText = (TEXT("GameOver"));
				FCanvasTextItem GameOverItem(FVector2D(HUDXRatio * 5.f, HUDYRatio * 5.f), FText::FromString(*resultText), HUDFont, FLinearColor::Yellow);
				GameOverItem.Scale = FVector2D(HUDYRatio * 5.f, HUDYRatio * 5.f);
				Canvas->DrawItem(GameOverItem);
			}
			

			//double avarangeTime = 0;
			//double fullTime = 0;
			//fullTime = std::accumulate(Vehicle->arrTime.begin(), Vehicle->arrTime.end(), 0.0);
			//if (Vehicle->arrTime.size() > 1)
			//	avarangeTime = std::accumulate(Vehicle->arrTime.begin(), Vehicle->arrTime.end(), 0.0) / Vehicle->arrTime.size();
			//Avarange time
			//FCanvasTextItem avarangeTimeTextItem(FVector2D(HUDXRatio * 5.f, HUDYRatio * 545.f), FText::FromString(*FString::FromInt(avarangeTime)), HUDFont, FLinearColor::White);
			//avarangeTimeTextItem.Scale = ScaleVec;
			//Canvas->DrawItem(avarangeTimeTextItem);

			//fullTime time
			//FCanvasTextItem fullTimeTextItem(FVector2D(HUDXRatio * 5.f, HUDYRatio * 590.f), FText::FromString(*FString::FromInt(fullTime)), HUDFont, FLinearColor::White);
			//fullTimeTextItem.Scale = ScaleVec;
			//Canvas->DrawItem(fullTimeTextItem);

			//Path of car in meters
			
		}
	}
}


#undef LOCTEXT_NAMESPACE
