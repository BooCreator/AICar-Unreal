// Fill out your copyright notice in the Description page of Project Settings.

#include "AIEnvironment.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "Misc/DateTime.h"


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
	if (!this->loaded) {
		this->count = this->readIntSetting("ai_cars", "count");
		for (int i = 0; i < this->count; i++) {
			FString a("ai_"); a.AppendInt(i);
			auto params = this->readStringSetting("ai_cars", a);
			if (params.Len() > 0) {
				AIParams ai_params(params);
				this->ai.Add(ai_params);
			}
		}
		this->loaded = true;
	}
}

void AIEnvironment::init(FString link, TArray <AMyPlayerController*> controllers) {
	this->Socket->Shutdown();
	this->Requests = std::queue<FString>();
	this->Messages = std::queue<FString>();
	this->myCarsController = controllers;
	this->reset = true;
	this->Socket = FWebSockets::JoyInit(link, this->Requests, this->Messages);
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
	if (this->Socket && !this->Socket->IsFinished()) {
		this->Socket->Stop();
		this->Socket = nullptr;
	}
}

void AIEnvironment::sync_tick(float DeltaSeconds, FString JSONPayload, uint32 request_ID) {
	if (this->count > 0)
		if (this->Socket && this->Socket->IsConnected()) {
			auto date = FDateTime::Now();
			UE_LOG(LogTemp, Warning, TEXT("Message send at %d.%d "), date.ToUnixTimestamp(), date.GetMillisecond())
			DataGuard.Lock();
			this->Requests.push(JSONPayload);
			DataGuard.Unlock();
			FGenericPlatformProcess::ConditionalSleep([&]() {return !this->Messages.empty(); });
			auto date2 = FDateTime::Now();
			UE_LOG(LogTemp, Warning, TEXT("Message get at %d.%d "), date2.ToUnixTimestamp(), date2.GetMillisecond())
			double cnt = (date2 - date).GetTotalMilliseconds() / 1000;
			UE_LOG(LogTemp, Warning, TEXT("Answer wait counter : %f sec"), cnt);
			if (this->Messages.size() > 0) {
				DataGuard.Lock();
				FString Message = this->Messages.front();
				this->Messages.pop();
				DataGuard.Unlock();
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
}