// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyWebSocketsModule.h"

#include "Lws/LwsWebSocketsManager.h"
typedef FLwsWebSocketsManager FPlatformWebSocketsManager;


DEFINE_LOG_CATEGORY(LogWebSockets);

// FMyWebSocketsModule
IMPLEMENT_MODULE(FMyWebSocketsModule, MyWebSockets);

FMyWebSocketsModule* FMyWebSocketsModule::Singleton = nullptr;

/*static*/ FString FMyWebSocketsModule::BuildUpgradeHeader(const TMap<FString, FString>& Headers) {
	FString HeaderString;
	for (const auto& OneHeader : Headers) {
		HeaderString += FString::Printf(TEXT("%s: %s\r\n"), *OneHeader.Key, *OneHeader.Value);
	}
	return HeaderString;
}

void FMyWebSocketsModule::StartupModule() {
	Singleton = this;
	const FString Protocols[] = {TEXT("ws")};
	WebSocketsManager = new FPlatformWebSocketsManager;
	WebSocketsManager->InitWebSockets(Protocols);
}

void FMyWebSocketsModule::ShutdownModule() {
	if (WebSocketsManager) {
		WebSocketsManager->ShutdownWebSockets();
		delete WebSocketsManager;
		WebSocketsManager = nullptr;
	}
	Singleton = nullptr;
}

FMyWebSocketsModule& FMyWebSocketsModule::Get() { return *Singleton; }

TSharedRef<IWebSocket> FMyWebSocketsModule::CreateWebSocket(const FString& Url, const TArray<FString>& Protocols, const TMap<FString, FString>& UpgradeHeaders)
{
	check(WebSocketsManager);
	TArray<FString> ProtocolsCopy = Protocols;
	ProtocolsCopy.RemoveAll([](const FString& Protocol){ return Protocol.IsEmpty(); });	
	TSharedRef<IWebSocket> WebSocket = WebSocketsManager->CreateWebSocket(Url, ProtocolsCopy, UpgradeHeaders);
	OnWebSocketCreated.Broadcast(WebSocket, Protocols, Url);
	return WebSocket;
}

TSharedRef<IWebSocket> FMyWebSocketsModule::CreateWebSocket(const FString& Url, const FString& Protocol, const TMap<FString, FString>& UpgradeHeaders)
{
	TArray<FString> Protocols;
	if (!Protocol.IsEmpty()) {
		Protocols.Add(Protocol);
	}
	return CreateWebSocket(Url, Protocols, UpgradeHeaders);
}
