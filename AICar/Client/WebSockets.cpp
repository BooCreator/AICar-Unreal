// Fill out your copyright notice in the Description page of Project Settings.

#include "WebSockets.h"

WebSockets::WebSockets() {
	if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
	{
		FModuleManager::Get().LoadModule("WebSockets");
	}
}

WebSockets::~WebSockets() { this->Close(); }

void WebSockets::Init(FString server, int ai_count) {
	this->Close();
	WebSockets_run = true;
	this->ServerURL = server;
	//if (ai_count > 0)
	//	this->connection = new std::thread(&WebSockets::doConnect);
}

void WebSockets::doConnect() {
	TSharedPtr<IWebSocket> Socket = FWebSocketsModule::Get().CreateWebSocket(this->ServerURL, this->ws);

	Socket->OnConnected().AddRaw(this, &WebSockets::OnConnected);
	Socket->OnConnectionError().AddRaw(this, &WebSockets::OnError);
	Socket->OnMessage().AddRaw(this, &WebSockets::OnMessage);

	while (WebSockets_run) {
		int sleep_time = 1;
		if (Socket->IsConnected()) {
			if (WebSockets_Requests.size() > 0) {
				FString JSONPayload = WebSockets_Requests.front();
				WebSockets_Requests.pop();
				Socket->Send(JSONPayload);
			}
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("WebSockets is not connected! Connecting..."));
			Socket->Connect();
			sleep_time = 1000;
		}
		Sleep(sleep_time);
	}
	Socket->Close();
}

void WebSockets::Close() {
	//if (this->connection) {
	//	WebSockets_run = false;
	//	this->connection->join();
	//	delete this->connection;
	//}
}

void WebSockets::OnConnected() {
	UE_LOG(LogTemp, Warning, TEXT("WebSockets connected!"));
}

void WebSockets::OnError(const FString & Message) {
	UE_LOG(LogTemp, Warning, TEXT("WebSockets error: %s!"), *Message);
}

void WebSockets::OnMessage(const FString & Message) {
	UE_LOG(LogTemp, Warning, TEXT("WebSockets on_message: %s"), *Message);
	WebSockets_Messages.push(Message);
}