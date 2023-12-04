// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class IWebSocket;
class IWebSocketsManager;

class WEBSOCKETS_API FMyWebSocketsModule : public IModuleInterface {
public:

	// FWebSocketModule
	FMyWebSocketsModule() : WebSocketsManager(nullptr) { }

	DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnWebSocketCreated, const TSharedPtr<class IWebSocket>& /* WebSocket */, 
											const TArray<FString>& /* Protocols */, const FString& /* Url */);
	FOnWebSocketCreated OnWebSocketCreated;
	
	static FMyWebSocketsModule& Get();

	virtual TSharedRef<IWebSocket> CreateWebSocket(const FString& Url, const TArray<FString>& Protocols, const TMap<FString, FString>& UpgradeHeaders = TMap<FString, FString>());

	virtual TSharedRef<IWebSocket> CreateWebSocket(const FString& Url, const FString& Protocol = FString(), const TMap<FString, FString>& UpgradeHeaders = TMap<FString, FString>());

private:
	static FString BuildUpgradeHeader(const TMap<FString, FString>& Headers);

	virtual void StartupModule() override;
	
	virtual void ShutdownModule() override;

	IWebSocketsManager* WebSocketsManager;
	friend class FLwsWebSocketsManager;
	friend class FLwsWebSocket;

	static FMyWebSocketsModule* Singleton;
};
