// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/HUD.h"
#include "AICarHud.generated.h"


UCLASS(config = Game)
class AAICarHud : public AHUD
{
	GENERATED_BODY()

public:
	AAICarHud();

	/** Font used to render the vehicle info */
	UPROPERTY()
	UFont* HUDFont;

	// Begin AHUD interface
	virtual void DrawHUD() override;
	// End AHUD interface
};
