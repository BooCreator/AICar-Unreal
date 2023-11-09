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
	this->Requests = std::queue<FString>();
	this->Messages = std::queue<FString>();
	this->myCarsController = controllers;
	this->reset = true;
	this->Socket = FWebSockets::JoyInit(link, &this->Requests, &this->Messages);
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
	if (this->Socket && !this->Socket->IsFinished())
		this->Socket->Stop();
}

void AIEnvironment::tick(float DeltaSeconds, FString JSONPayload) {
	if (this->Requests.size() == 0)
		this->Requests.push(JSONPayload);
	//while(!this->Messages.size()) {
	//	FPlatformProcess::Sleep(0.001f);
	//}
	if (this->Messages.size() > 0) {
		FString Message = this->Messages.front();
		this->Messages.pop();
		FActions actions = this->Loader.ParseJSON(Message);
		int n = (this->myCarsController.Num() < actions.cars.Num()) ? this->myCarsController.Num() : actions.cars.Num();
		for (int i = 0; i < n; i++)
		{
			auto car = this->myCarsController[i];
			car->setForward(actions.cars[i].gas);
			car->setRight(actions.cars[i].rotate);
		}
	}
}