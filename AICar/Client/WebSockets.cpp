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

void WebSockets::Close() {
	WebSockets_run = false;
	//if (this->connection) {
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
	//WebSockets_Messages.push(Message);
}