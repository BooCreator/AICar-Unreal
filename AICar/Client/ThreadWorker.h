// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

class FThreadWorker : public FRunnable
{   
    static  FThreadWorker* Runnable;
    
    FRunnableThread* Thread;
    
    FString url;
    TArray* In;
    TArray* Out;
    
    FThreadSafeCounter StopTaskCounter;

public:

    bool IsFinished() const { return PrimesFoundCount >= TotalPrimesToFind; }
    
    FThreadWorker(FString Url, TArray& In, TArray& Out);
    virtual ~FThreadWorker();

    // Begin FRunnable interface.
    virtual bool Init();
    virtual uint32 Run();
    virtual void Stop();

    void EnsureCompletion();
    
    static FThreadWorker* JoyInit(FString Url, TArray& In, TArray& Out);
    static void Shutdown();
    static bool IsThreadFinished();

};