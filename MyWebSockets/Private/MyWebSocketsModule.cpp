// YourModuleNameModule.cpp
#include "MyWebSocketsModule.h"

#include "Lws/LwsWebSocketsManager.h"
typedef FLwsWebSocketsManager FPlatformWebSocketsManager;

IMPLEMENT_MODULE(FMyWebSocketsModule, MyWebSockets);

FMyWebSocketsModule* FMyWebSocketsModule::Singleton = nullptr;

FMyWebSocketsModule& FMyWebSocketsModule::Get() { return *Singleton; }

void FMyWebSocketsModule::StartupModule()
{
	Singleton = this;
	const FString Protocols[] = { TEXT("ws") };
	WebSocketsManager = new FPlatformWebSocketsManager;
	WebSocketsManager->InitWebSockets(Protocols);
}

void FMyWebSocketsModule::ShutdownModule()
{
	if (WebSocketsManager) {
		WebSocketsManager->ShutdownWebSockets();
		delete WebSocketsManager;
		WebSocketsManager = nullptr;
	}
	Singleton = nullptr;
}

TSharedRef<IWebSocket> FMyWebSocketsModule::CreateWebSocket(const FString& Url, const TArray<FString>& Protocols, const TMap<FString, FString>& UpgradeHeaders)
{
	check(WebSocketsManager);
	TArray<FString> ProtocolsCopy = Protocols;
	ProtocolsCopy.RemoveAll([](const FString& Protocol) { return Protocol.IsEmpty(); });
	TSharedRef<IWebSocket> WebSocket = WebSocketsManager->CreateWebSocket(Url, ProtocolsCopy, UpgradeHeaders);
	OnWebSocketCreated.Broadcast(WebSocket, Protocols, Url);
	return WebSocket;
}

TSharedRef<IWebSocket> FMyWebSocketsModule::CreateWebSocket(const FString& Url, const FString& Protocol, const TMap<FString, FString>& UpgradeHeaders)
{
	TArray<FString> Protocols;
	if (!Protocol.IsEmpty()) { Protocols.Add(Protocol); }
	return CreateWebSocket(Url, Protocols, UpgradeHeaders);
}