// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "IWebSocketsManager.h"
#include "MyWebSocket.h"
#include "HAL/Runnable.h"
#include "Misc/SingleThreadRunnable.h"
#include "HAL/ThreadSafeCounter.h"

#include "Windows/AllowWindowsPlatformTypes.h"

//#include "libwebsockets.h"

#include "Windows/HideWindowsPlatformTypes.h"

/**
 * 
 */
class AICAR_API MyWebSocketsManager
{
public:
	MyWebSocketsManager();
	~MyWebSocketsManager();
};
