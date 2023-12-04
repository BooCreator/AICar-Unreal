// YourModuleNameModule.h
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class IWebSocket;
class IWebSocketsManager;

class MYWEBSOCKETS_API FMyWebSocketsModule : public IModuleInterface
{
public:
	FMyWebSocketsModule() : WebSocketsManager(nullptr) { }

	DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnWebSocketCreated, const TSharedPtr<class IWebSocket>& /* WebSocket */, const TArray<FString>& /* Protocols */, const FString& /* Url */);
	FOnWebSocketCreated OnWebSocketCreated;

	static FMyWebSocketsModule& Get();

	virtual TSharedRef<IWebSocket> CreateWebSocket(const FString& Url, const TArray<FString>& Protocols, const TMap<FString, FString>& UpgradeHeaders = TMap<FString, FString>());
	virtual TSharedRef<IWebSocket> CreateWebSocket(const FString& Url, const FString& Protocol = FString(), const TMap<FString, FString>& UpgradeHeaders = TMap<FString, FString>());

private:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** Manages active web sockets */
	IWebSocketsManager* WebSocketsManager;
	friend class FLwsWebSocketsManager;
	friend class FLwsWebSocket;

	/** singleton for the module while loaded and available */
	static FMyWebSocketsModule* Singleton;
};