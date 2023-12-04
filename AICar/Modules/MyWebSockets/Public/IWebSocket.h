// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"

class IWebSocket {
public:

	virtual ~IWebSocket() { }

	virtual void Connect() = 0;

	virtual void Close(int32 Code = 1000, const FString& Reason = FString()) = 0;

	virtual bool IsConnected() = 0;
	
	virtual void Send(const FString& Data) = 0;

	virtual void Send(const void* Data, SIZE_T Size, bool bIsBinary = false) = 0;

	DECLARE_EVENT(IWebSocket, FWebSocketConnectedEvent);
	virtual FWebSocketConnectedEvent& OnConnected() = 0;

	DECLARE_EVENT_OneParam(IWebSocket, FWebSocketConnectionErrorEvent, const FString& /* Error */);
	virtual FWebSocketConnectionErrorEvent& OnConnectionError() = 0;

	DECLARE_EVENT_ThreeParams(IWebSocket, FWebSocketClosedEvent, int32 /* StatusCode */, const FString& /* Reason */, bool /* bWasClean */);
	virtual FWebSocketClosedEvent& OnClosed() = 0;

	DECLARE_EVENT_OneParam(IWebSocket, FWebSocketMessageEvent, const FString& /* MessageString */);
	virtual FWebSocketMessageEvent& OnMessage() = 0;

	DECLARE_EVENT_ThreeParams(IWebSocket, FWebSocketRawMessageEvent, const void* /* Data */, SIZE_T /* Size */, SIZE_T /* BytesRemaining */);
	virtual FWebSocketRawMessageEvent& OnRawMessage() = 0;

	DECLARE_EVENT_OneParam(IWebSocket, FWebSocketMessageSentEvent, const FString& /* MessageString */);
	virtual FWebSocketMessageSentEvent& OnMessageSent() = 0;
};