// Fill out your copyright notice in the Description page of Project Settings.

#include "WebSockets.h"

FWebSockets* FWebSockets::Runnable = NULL;

FWebSockets::FWebSockets(FString Url, std::queue<FString>& In, std::queue<FString>& Out) : StopTaskCounter(0) {
	if (!FModuleManager::Get().IsModuleLoaded("WebSockets")) {
		FModuleManager::Get().LoadModule("WebSockets");
	}

    this->url = Url;
    this->in = &In;
    this->out = &Out;
    
    Thread = FRunnableThread::Create(this, TEXT("WebSockets"), 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
}

FWebSockets::~FWebSockets() {
    delete Thread;
    Thread = NULL;
}

bool FWebSockets::Init() { 
    return true; 
}

uint32 FWebSockets::Run()
{
    TSharedPtr<IWebSocket> Socket = FWebSocketsModule::Get().CreateWebSocket(this->url, TEXT("ws"));

	Socket->OnConnected().AddRaw(this, &FWebSockets::OnConnected);
	Socket->OnConnectionError().AddRaw(this, &FWebSockets::OnError);
	Socket->OnMessage().AddRaw(this, &FWebSockets::OnMessage);

    while (! IsFinished())
    {
        float sleep_time = 1.f;
		if (Socket->IsConnected()) {
			this->connected = true;
            sleep_time = 0.001f;
			if (this->in->size() > 0) {
				FString JSONPayload = this->in->front();
				this->in->pop();
				Socket->Send(JSONPayload);
			}
		} else {
			UE_LOG(LogTemp, Warning, TEXT("WebSockets is not connected! Connecting..."));
			Socket->Connect();
		}
		FPlatformProcess::Sleep(sleep_time);
    }
    Socket->Close();
	this->connected = false;
    return 0;
}

void FWebSockets::Stop() { 
    StopTaskCounter.Increment(); 
}

FWebSockets* FWebSockets::JoyInit(FString Url, std::queue<FString>& In, std::queue<FString>& Out) {
    if (!Runnable) {
        Runnable = new FWebSockets(Url, In, Out);         
    }
    return Runnable;
}

void FWebSockets::EnsureCompletion() {
    Stop();
    Thread->WaitForCompletion();
}

void FWebSockets::Shutdown() {
    if (Runnable) {
        Runnable->EnsureCompletion();
        delete Runnable;
        Runnable = NULL;
    }
}

bool FWebSockets::IsThreadFinished() {
    if(Runnable) return Runnable->IsFinished();
    return true;
}

void FWebSockets::OnConnected() {
	UE_LOG(LogTemp, Warning, TEXT("WebSockets connected!"));
}

void FWebSockets::OnError(const FString & Message) {
	UE_LOG(LogTemp, Warning, TEXT("WebSockets error: %s!"), *Message);
}

void FWebSockets::OnMessage(const FString & Message) {
	UE_LOG(LogTemp, Warning, TEXT("WebSockets on_message: %s"), *Message);
	this->out->push(Message);
}