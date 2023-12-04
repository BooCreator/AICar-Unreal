// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SharedPointer.h"
#include "Containers/ArrayView.h"

class IWebSocket;

class IWebSocketsManager {
public:

	virtual void InitWebSockets(TArrayView<const FString> Protocols) = 0;

	virtual void ShutdownWebSockets() = 0;

	virtual TSharedRef<IWebSocket> CreateWebSocket(const FString& Url, const TArray<FString>& Protocols, const TMap<FString, FString>& UpgradeHeaders) = 0;

	virtual ~IWebSocketsManager() = default;
};
