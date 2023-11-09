// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "JsonLoader.h"
#include "WebSockets.h"
#include "../MyPlayerController.h"
#include "CoreMinimal.h"


struct Colors {
public:
	UMaterialInterface* Black = FindObject<UMaterialInterface>(NULL,
		TEXT("MaterialInstanceConstant'/Game/Vehicle/Sedan/Materials/Black.Black'"));
	UMaterialInterface* Blue = FindObject<UMaterialInterface>(NULL,
		TEXT("Material'/Game/Vehicle/Sedan/Materials/Blue.Blue'"));
	UMaterialInterface* DarkBlue = FindObject<UMaterialInterface>(NULL,
		TEXT("MaterialInstanceConstant'/Game/Vehicle/Sedan/Materials/Dark_blue.Dark_blue'"));
	UMaterialInterface* Gray = FindObject<UMaterialInterface>(NULL,
		TEXT("MaterialInstanceConstant'/Game/Vehicle/Sedan/Materials/Gray.Gray'"));
	UMaterialInterface* Green = FindObject<UMaterialInterface>(NULL,
		TEXT("MaterialInstanceConstant'/Game/Vehicle/Sedan/Materials/Green.Green'"));
	UMaterialInterface* Glass = FindObject<UMaterialInterface>(NULL,
		TEXT("MaterialInstanceConstant'/Game/Vehicle/Sedan/Materials/M_Vehicle_Sedan_Inst_Glass.M_Vehicle_Sedan_Inst_Glass'"));
	UMaterialInterface* Tires = FindObject<UMaterialInterface>(NULL,
		TEXT("MaterialInstanceConstant'/Game/Vehicle/Sedan/Materials/M_Vehicle_Sedan_Inst_Tires.M_Vehicle_Sedan_Inst_Tires'"));
	UMaterialInterface* Metal = FindObject<UMaterialInterface>(NULL,
		TEXT("MaterialInstanceConstant'/Game/Vehicle/Sedan/Materials/Melal.Melal'"));
	UMaterialInterface* Red = FindObject<UMaterialInterface>(NULL,
		TEXT("MaterialInstanceConstant'/Game/Vehicle/Sedan/Materials/Red.Red'"));
	UMaterialInterface* Yellow = FindObject<UMaterialInterface>(NULL,
		TEXT("MaterialInstanceConstant'/Game/Vehicle/Sedan/Materials/Yellow.Yellow'"));
	TArray<UMaterialInterface*> Array = { this->Blue, this->DarkBlue, this->Gray, this->Green, this->Glass, this->Tires, this->Metal, this->Red, this->Yellow };

	UMaterialInterface* getColor(FString name) {
		if (name == "black") {
			return this->Black;
		}
		else if (name == "blue") {
			return this->Blue;
		}
		else if (name == "dark_blue") {
			return this->DarkBlue;
		}
		else if (name == "gray") {
			return this->Gray;
		}
		else if (name == "green") {
			return this->Green;
		}
		else if (name == "glass") {
			return this->Glass;
		}
		else if (name == "tires") {
			return this->Tires;
		}
		else if (name == "metal") {
			return this->Metal;
		}
		else if (name == "red") {
			return this->Red;
		}
		else if (name == "yellow") {
			return this->Yellow;
		}
		else {
			return this->Blue;
		}

	}
};


struct AIParams {
public:
	FString Type;
	UMaterialInterface* Color;
	int Num = 0;
	Colors c;

	AIParams(FString Values) {
		FString value = Values;
		FString l; FString r; FString v;
		while (true) {
			if (value.Split(",", &l, &r)) {
				if (l.Split("=", &l, &v)) {
					if (l == "Type") {
						this->Type = v;
					}
					else if (l == "Color") {
						this->Color = c.getColor(v);
					}
					else if (l == "Num") {
						this->Num = FCString::Atoi(*v);
					}
				}
				value = r;
			}
			else { break; }
		}
		if (r.Split("=", &l, &v)) {
			if (l == "Type") {
				this->Type = v;
			}
			else if (l == "Color") {
				this->Color = c.getColor(v);
			}
			else if (l == "Num") {
				this->Num = FCString::Atoi(*v);
			}
		}
	}
};


class AICAR_API AIEnvironment
{
private:
	FActions lastActions;
	char isState = 0; // send
	int delayForSlowCounter = 0;
	int currentForSlowCounter = 0;
	JsonLoader Loader;
	WebSockets Socket;
public:
	AIEnvironment();
	~AIEnvironment();

	void load_ai();

	void init(FString link, TArray<AMyPlayerController*> controllers);

	void tick(float DeltaSeconds, FString JSONPayload);

	FString readStringSetting(FString section, FString param);
	float readFloatSetting(FString section, FString param);
	int readIntSetting(FString section, FString param);

	UMaterialInterface* get_color(int i);
	TArray<int> get_kinds(int n);

	void dispose();
public:
	TArray<AIParams> ai;
	bool reset = true;
	int count = 1;
	TArray<AMyPlayerController*> myCarsController;
};
