// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WebSocketsModule.h" // Module definition
#include "IWebSocket.h"       // Socket definition

#include "Modules/ModuleManager.h"

#include "HAL/Runnable.h"

#include "CoreMinimal.h"

class AICAR_API FWebSockets : public FRunnable{
private:
	static  FWebSockets* Runnable;
    
    FRunnableThread* Thread;
    
    FString url;
    TArray<FString> in;
    TArray<FString> out;
    
    FThreadSafeCounter StopTaskCounter;
public:
	bool IsFinished() const { return false; }
    
    FWebSockets(FString Url, TArray<FString>& In, TArray<FString>& Out);
    virtual ~FWebSockets();

    // Begin FRunnable interface.
    virtual bool Init();
    virtual uint32 Run();
    virtual void Stop();

    void EnsureCompletion();
    
    static FWebSockets* JoyInit(FString Url, TArray<FString>& In, TArray<FString>& Out);
    static void Shutdown();
    static bool IsThreadFinished();

	void OnMessage(const FString & Message);
	void OnError(const FString & Message);
	void OnConnected();
};
