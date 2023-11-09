// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "WebSocketsModule.h" // Module definition
#include "IWebSocket.h"       // Socket definition

//#include <thread>
//#include <windows.h>

#include "Modules/ModuleManager.h"

#include <queue>
#include "CoreMinimal.h"

bool WebSockets_run = false;

class AICAR_API WebSockets {
private:
	FString ServerURL = TEXT("ws://127.0.0.1/");
	FString ws = TEXT("ws");
	//std::thread* connection = nullptr;
public:
	static std::queue<FString> WebSockets_Requests;
	static std::queue<FString> WebSockets_Messages;
private:
	void doConnect();
public:
	WebSockets();
	~WebSockets();

	void Init(FString server, int ai_count);

	void Close();

	void OnMessage(const FString & Message);
	void OnError(const FString & Message);
	void OnConnected();
};
