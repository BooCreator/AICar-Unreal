// Fill out your copyright notice in the Description page of Project Settings.

#include "PhysicsEngine/PhysicsSettings.h"
#include "AIEnvironment.h"

AIEnvironment::AIEnvironment() {
}

AIEnvironment::~AIEnvironment() {
}

FString AIEnvironment::readStringSetting(FString section, FString param) {
	FString temp;
	if (GConfig->GetString(*section, *param, temp, GGameIni))
	{
		UE_LOG(LogTemp, Warning, TEXT("read from ini : %s"), *temp);
	}
	return temp;
}

float AIEnvironment::readFloatSetting(FString section, FString param) {
	float value;
	if (GConfig->GetFloat(*section, *param, value, GGameIni))
	{
		UE_LOG(LogTemp, Warning, TEXT("read from ini : %d"), value);
	}
	return value;
}

int AIEnvironment::readIntSetting(FString section, FString param) {
	int value;
	if (GConfig->GetInt(*section, *param, value, GGameIni))
	{
		UE_LOG(LogTemp, Warning, TEXT("read from ini : %d"), value);
	}
	return value;
}

void AIEnvironment::load_ai() {
	this->count = this->readIntSetting("ai_cars", "count");
	for (int i = 0; i < this->count; i++) {
		FString a("ai_"); a.AppendInt(i);
		auto params = this->readStringSetting("ai_cars", a);
		if (params.Len() > 0) {
			AIParams ai_params(params);
			this->ai.Add(ai_params);
		}
	}
}

void AIEnvironment::init(FString link, TArray <AMyPlayerController*> controllers) {
	this->myCarsController = controllers;
	this->Socket.Init(link, this->count);
	this->reset = true;
}

UMaterialInterface* AIEnvironment::get_color(int i) {
	int ai_n = this->ai.Num(); int c = 0;
	for (int n = 0; n < this->ai.Num(); n++) {
		if (i - c < this->ai[n].Num) {
			ai_n = n;
			break;
		}
		else { c += this->ai[n].Num; }
	}
	if (this->ai.Num() > ai_n) {
		return this->ai[ai_n].Color;
	}
	else { return nullptr; }
}

TArray<int> AIEnvironment::get_kinds(int n) {
	TArray<int> res;
	int c = 0;
	for (int i = 0; i < this->ai.Num() && i < n; i++) {
		for (int j = 0; j < this->ai[i].Num; j++) {
			res.Add(i + 1);
			c++;
		}
	}
	for (int i = c; i < n; i++) {
		res.Add(-1);
	}
	return res;
}

void AIEnvironment::dispose() {

}

void AIEnvironment::tick(float DeltaSeconds, FString JSONPayload) {
	//if (this->Socket.WebSockets_Messages.size() > 0) {
	//	FString Message = this->Socket.WebSockets_Messages.front();
	//	this->Socket.WebSockets_Messages.pop();
	//	FActions result = this->Loader.ParseJSON(Message);
	//}
	//if (this->isState == 0 && this->count > 0) {
	//	if (!this->Connection.isConnected()) {
	//		UE_LOG(LogTemp, Warning, TEXT("WebSockets is not connected! Connecting..."));
	//		this->Connection.Connect();
	//	}
	//	else {
	//		this->Connection.Send(JSONPayload);
	//		this->isState = 1; // wait
	//		UE_LOG(LogTemp, Warning, TEXT("Send success! Use static: %d"), (this->isInited) ? 0 : 1);
	//	}
	//	this->currentForSlowCounter += DeltaSeconds * 1000;
	//}
	//else if (this->isState == 1) {
	//	if (this->Connection.hasMessage()) {
	//		this->lastActions = this->Connection.popMessage();
	//		this->isState = 2; // process
	//		this->isInited = true;
	//	}
	//}
	//if (this->isState == 2) {
	//	if (this->currentForSlowCounter < this->delayForSlowCounter) {
	//		this->currentForSlowCounter += DeltaSeconds * 1000;
	//	}
	//	else {
	//		this->isState = 0;
	//		this->currentForSlowCounter = 0;
	//	}
	//	if (this->lastActions.params.fps > 0) {
	//		GEngine->FixedFrameRate = this->lastActions.params.fps;
	//		GEngine->bUseFixedFrameRate = true;
	//	}
	//	else {
	//		GEngine->bUseFixedFrameRate = false;
	//	}
	//	UPhysicsSettings* PhysSetting = UPhysicsSettings::Get();
	//	if (this->lastActions.params.phys_fps > 0) {
	//		PhysSetting->custom_substepping = true;
	//		PhysSetting->phys_fps = this->lastActions.params.phys_fps;
	//		PhysSetting->speed_up = this->lastActions.params.speed_up;
	//	}
	//	else {
	//		PhysSetting->custom_substepping = false;
	//	}
//
	//	int count = (this->myCarsController.Num() < this->lastActions.cars.Num()) ? this->myCarsController.Num() : this->lastActions.cars.Num();
	//	for (int i = 0; i < count; i++)
	//	{
	//		auto car = this->myCarsController[i];
	//		car->setForward(this->lastActions.cars[i].gas);
	//		car->setRight(this->lastActions.cars[i].rotate);
	//	}
	//}
}