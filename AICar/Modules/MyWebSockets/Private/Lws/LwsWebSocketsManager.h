// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IWebSocketsManager.h"
#include "LwsWebSocket.h"
#include "HAL/Runnable.h"
#include "Misc/SingleThreadRunnable.h"
#include "HAL/ThreadSafeCounter.h"

#include "Windows/AllowWindowsPlatformTypes.h"

THIRD_PARTY_INCLUDES_START
#include "libwebsockets.h"
THIRD_PARTY_INCLUDES_END

#include "Windows/HideWindowsPlatformTypes.h"

class FRunnableThread;

class FLwsWebSocketsManager : 
	public IWebSocketsManager, public FRunnable , public FSingleThreadRunnable
{
public:
	FLwsWebSocketsManager();

	static FLwsWebSocketsManager& Get();

	void StartProcessingWebSocket(FLwsWebSocket* Socket);

	// IWebSocketsManager
	virtual void InitWebSockets(TArrayView<const FString> Protocols) override;
	virtual void ShutdownWebSockets() override;
	virtual TSharedRef<IWebSocket> CreateWebSocket(const FString& Url, const TArray<FString>& Protocols, const TMap<FString, FString>& UpgradeHeaders) override;

private:

	//~ Begin FRunnable Interface
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;
	//~ End FRunnable Interface
	
	virtual class FSingleThreadRunnable* GetSingleThreadInterface() override { return this; }

	virtual void Tick() override;

	// FLwsWebSocketsManager
	bool GameThreadTick(float DeltaTime);
	static int StaticCallbackWrapper(lws* Connection, lws_callback_reasons Reason, void* UserData, void* Data, size_t Length);
	int CallbackWrapper(lws* Connection, lws_callback_reasons Reason, void* UserData, void* Data, size_t Length);

	lws_context* LwsContext;
	TArray<lws_protocols> LwsProtocols;

	TArray<TSharedRef<FLwsWebSocket>> Sockets;
	TArray<FLwsWebSocket*> SocketsTickingOnThread;
	TQueue<FLwsWebSocket*> SocketsToStart;
	TQueue<FLwsWebSocket*> SocketsToStop;
	TArray<FLwsWebSocket*> SocketsDestroyedDuringService;

	FDelegateHandle TickHandle;

	FRunnableThread* Thread;
	FThreadSafeCounter ExitRequest;
	double ThreadTargetFrameTimeInSeconds;
	double ThreadMinimumSleepTimeInSeconds;
};
