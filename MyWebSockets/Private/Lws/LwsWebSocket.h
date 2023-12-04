// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "Containers/Queue.h"

#include "IWebSocket.h"

#include "Windows/WindowsHWrapper.h"
#include "Windows/AllowWindowsPlatformTypes.h"

// ссылка на неразрешенный внешний символ deflate в функции zlib_stateful_compress_block

THIRD_PARTY_INCLUDES_START
#include "libwebsockets.h"		
THIRD_PARTY_INCLUDES_END

#include "Windows/HideWindowsPlatformTypes.h"

struct FLwsSendBuffer {
	FLwsSendBuffer(const uint8* Data, const SIZE_T Size, const bool bInIsBinary);

	int32 GetPayloadSize() const;

	bool IsDone() const { return !HasError() && BytesWritten >= GetPayloadSize(); }

	bool HasError() const { return bHasError; }

	const bool bIsBinary;
	int32 BytesWritten;
	TArray<uint8> Payload;
	bool bHasError;
};

struct FLwsReceiveBufferBinary {
	FLwsReceiveBufferBinary(const uint8* Data, const int32 Size, const int32 InBytesRemaining);

	TArray<uint8> Payload;
	const int32 BytesRemaining;
};

typedef TUniquePtr<FLwsReceiveBufferBinary> FLwsReceiveBufferBinaryPtr;

struct FLwsReceiveBufferText {
	FLwsReceiveBufferText(FString&& InText);

	const FString Text;
};

typedef TUniquePtr<FLwsReceiveBufferText> FLwsReceiveBufferTextPtr;

class FLwsWebSocket
	: public IWebSocket, public TSharedFromThis<FLwsWebSocket>
{
public:
	virtual ~FLwsWebSocket();

	virtual void Connect() override;
	virtual void Close(const int32 Code = 1000, const FString& Reason = FString()) override;

	virtual bool IsConnected() override
		{ return LastGameThreadState == EState::Connected || LastGameThreadState == EState::ClosingByRequest; }

	virtual void Send(const FString& Data);
	virtual void Send(const void* Data, SIZE_T Size, bool bIsBinary) override;

	DECLARE_DERIVED_EVENT(FLwsWebSocket, IWebSocket::FWebSocketConnectedEvent, FWebSocketConnectedEvent);
	virtual FWebSocketConnectedEvent& OnConnected() override { return ConnectedEvent; }

	DECLARE_DERIVED_EVENT(FLwsWebSocket, IWebSocket::FWebSocketConnectionErrorEvent, FWebSocketConnectionErrorEvent);
	virtual FWebSocketConnectionErrorEvent& OnConnectionError() override { return ConnectionErrorEvent; }

	DECLARE_DERIVED_EVENT(FLwsWebSocket, IWebSocket::FWebSocketClosedEvent, FWebSocketClosedEvent);
	virtual FWebSocketClosedEvent& OnClosed() override { return ClosedEvent; }

	DECLARE_DERIVED_EVENT(FLwsWebSocket, IWebSocket::FWebSocketMessageEvent, FWebSocketMessageEvent);
	virtual FWebSocketMessageEvent& OnMessage() override { return MessageEvent; }

	DECLARE_DERIVED_EVENT(FLwsWebSocket, IWebSocket::FWebSocketRawMessageEvent, FWebSocketRawMessageEvent);
	virtual FWebSocketRawMessageEvent& OnRawMessage() override { return RawMessageEvent; }

	DECLARE_DERIVED_EVENT(FLwsWebSocket, IWebSocket::FWebSocketMessageSentEvent, FWebSocketMessageSentEvent);
	virtual FWebSocketMessageSentEvent& OnMessageSent() override { return OnMessageSentEvent; }

	int LwsCallback(lws* Instance, lws_callback_reasons Reason, void* Data, size_t Length);

	void GameThreadTick();
	void GameThreadFinalize();
	bool LwsThreadInitialize(struct lws_context &LwsContext);
	void LwsThreadTick();

private:
	FLwsWebSocket(const FString& Url, const TArray<FString>& Protocols, const FString& UpgradeHeader);

	friend class SharedPointerInternals::TIntrusiveReferenceController<FLwsWebSocket>;
	void ConnectInternal(struct lws_context &LwsContext);
	void SendFromQueue();
	void ClearData();
	bool WriteBuffer(FLwsSendBuffer& Buffer);

private:
	FCriticalSection StateLock;

	enum class EState : uint8 {
		/** Constructed, nothing to do */
		None,
		/** Awaiting connection start */
		StartConnecting,
		/** Connecting */
		Connecting,
		/** Connected */
		Connected,
		/** Closing (self initiated) */
		ClosingByRequest,
		/** Closed */
		Closed,
		/** Errored, nothing to do */
		Error,
	};
	
	static const TCHAR* ToString(const EState InState);

	struct FClosedReason {
		/** Descriptive reason for the state change */
		FString Reason;
		/** Close status (for State=Closed) */
		uint16 CloseStatus;
		/** Was close clean? (for State=Closed) */
		bool bWasClean;
	};

	EState State;
	FClosedReason ClosedReason;
	EState LastGameThreadState;

	struct FCloseRequest {
		/** Constructor */
		FCloseRequest() : Code(0), Reason(nullptr) {}

		/** Code specified when calling Close() */
		int32 Code;
		/** Reason specified when calling Close(), converted to ANSICHAR for libwebsockets */
		ANSICHAR *Reason;
	};

	FCloseRequest CloseRequest;

	bool bWasSendQueueEmpty;

	FWebSocketConnectedEvent ConnectedEvent;
	FWebSocketConnectionErrorEvent ConnectionErrorEvent;
	FWebSocketClosedEvent ClosedEvent;
	FWebSocketMessageEvent MessageEvent;
	FWebSocketRawMessageEvent RawMessageEvent;
	FWebSocketMessageSentEvent OnMessageSentEvent;

	struct lws *LwsConnection;
	FString Url;
	TArray<FString> Protocols;
	FString UpgradeHeader;

	bool bWantsMessageEvents;

	bool bWantsRawMessageEvents;

	FString ReceiveBuffer;

	static int32 IncrementingIdentifier;
	const int32 Identifier;

public:
	TQueue<FLwsSendBuffer*, EQueueMode::Spsc> SendQueue;
	TQueue<FLwsReceiveBufferBinaryPtr, EQueueMode::Spsc> ReceiveBinaryQueue;
	TQueue<FLwsReceiveBufferTextPtr, EQueueMode::Spsc> ReceiveTextQueue;
};

typedef TSharedRef<FLwsWebSocket> FLwsWebSocketRef;
