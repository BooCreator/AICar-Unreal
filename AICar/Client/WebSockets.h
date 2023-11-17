// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WebSocketsModule.h" // Module definition
#include "IWebSocket.h"       // Socket definition

//#include "Lws/LwsWebSocketsManager.h"

#include "Modules/ModuleManager.h"

#include "HAL/Runnable.h"

#include <queue>

#include "CoreMinimal.h"

class AICAR_API FWebSockets : public FRunnable {
private:
	static FWebSockets* Runnable;
    
    FRunnableThread* Thread;
    
    FString url;
    std::queue<FString>* in;
    std::queue<FString>* out;

	bool connected = false;

public:
    FThreadSafeCounter StopTaskCounter;
public:
	bool IsFinished() const { return StopTaskCounter.GetValue() > 0; }
    bool IsConnected() const { return connected; }

    FWebSockets(FString Url, std::queue<FString>& In, std::queue<FString>& Out);
    virtual ~FWebSockets();

    // Begin FRunnable interface.
    virtual bool Init();
    virtual uint32 Run();
    virtual void Stop();

    void EnsureCompletion();
    
    static FWebSockets* JoyInit(FString Url, std::queue<FString>& In, std::queue<FString>& Out);
    static void Shutdown();
    static bool IsThreadFinished();

	void OnMessage(const FString & Message);
	void OnError(const FString & Message);
	void OnConnected();
};
