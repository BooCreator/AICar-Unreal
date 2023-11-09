// Fill out your copyright notice in the Description page of Project Settings.


#include "ThreadWorker.h"


FThreadWorker* FThreadWorker::Runnable = NULL;

FThreadWorker::FThreadWorker(FString Url, TArray<FString>& In, TArray<FString>& Out) : StopTaskCounter(0) {
	if (!FModuleManager::Get().IsModuleLoaded("WebSockets")) {
		FModuleManager::Get().LoadModule("WebSockets");
	}

    this->url = Url;
    this->in = In;
    this->out = Out;
    
    Thread = FRunnableThread::Create(this, TEXT("FThreadWorker"), 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
}

FThreadWorker::~FThreadWorker() {
    delete Thread;
    Thread = NULL;
}

bool FThreadWorker::Init() { 
    return true; 
}

uint32 FThreadWorker::Run()
{
    TSharedPtr<IWebSocket> Socket = FWebSocketsModule::Get().CreateWebSocket(this->url, TEXT("ws"));

	Socket->OnConnected().AddRaw(this, &FThreadWorker::OnConnected);
	Socket->OnConnectionError().AddRaw(this, &FThreadWorker::OnError);
	Socket->OnMessage().AddRaw(this, &FThreadWorker::OnMessage);

    while (StopTaskCounter.GetValue() == 0 && ! IsFinished())
    {
        float sleep_time = 1.f;
		if (Socket->IsConnected()) {
            sleep_time = 0.001f;
			//if (WebSockets_Requests.size() > 0) {
			//	FString JSONPayload = WebSockets_Requests.front();
			//	WebSockets_Requests.pop();
			//	Socket->Send(JSONPayload);
			//}
		} else {
			UE_LOG(LogTemp, Warning, TEXT("WebSockets is not connected! Connecting..."));
			Socket->Connect();
		}
		FPlatformProcess::Sleep(sleep_time);
    }
    Socket->Close();
    return 0;
}

void FThreadWorker::Stop() { 
    StopTaskCounter.Increment(); 
}

FThreadWorker* FThreadWorker::JoyInit(FString Url, TArray<FString>& In, TArray<FString>& Out) {
    if (!Runnable) {
        Runnable = new FThreadWorker(Url, In, Out);         
    }
    return Runnable;
}

void FThreadWorker::EnsureCompletion() {
    Stop();
    Thread->WaitForCompletion();
}

void FThreadWorker::Shutdown() {
    if (Runnable) {
        Runnable->EnsureCompletion();
        delete Runnable;
        Runnable = NULL;
    }
}

bool FThreadWorker::IsThreadFinished() {
    if(Runnable) return Runnable->IsFinished();
    return true;
}

void FThreadWorker::OnConnected() {
	UE_LOG(LogTemp, Warning, TEXT("WebSockets connected!"));
}

void FThreadWorker::OnError(const FString & Message) {
	UE_LOG(LogTemp, Warning, TEXT("WebSockets error: %s!"), *Message);
}

void FThreadWorker::OnMessage(const FString & Message) {
	UE_LOG(LogTemp, Warning, TEXT("WebSockets on_message: %s"), *Message);
	//WebSockets_Messages.push(Message);
}