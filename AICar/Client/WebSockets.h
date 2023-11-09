// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include <queue>
#include "CoreMinimal.h"

//std::queue<FString> WebSockets_Requests;
//std::queue<FString> WebSockets_Messages;

class AICAR_API WebSockets {
private:
	FString ServerURL = TEXT("ws://127.0.0.1/");
	FString ws = TEXT("ws");
	//std::thread* connection = nullptr;
public:
	WebSockets();
	~WebSockets();

	void Init(FString server, int ai_count);

	void Close();
};
