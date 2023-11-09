// Fill out your copyright notice in the Description page of Project Settings.

#include "WebSockets.h"

WebSockets::WebSockets() { }

WebSockets::~WebSockets() { this->Close(); }

void WebSockets::Init(FString server, int ai_count) {
	this->Close();
	this->ServerURL = server;
}

void WebSockets::Close() { }