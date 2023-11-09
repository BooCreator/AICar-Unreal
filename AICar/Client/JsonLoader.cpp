// Fill out your copyright notice in the Description page of Project Settings.


#include "JsonLoader.h"

JsonLoader::JsonLoader() { }

JsonLoader::~JsonLoader() { }

FActions JsonLoader::ParseJSON(FString Message) {
	FActions CarInfo;
	if (!FJsonObjectConverter::JsonObjectStringToUStruct(Message, &CarInfo, 0, 0)) {
		UE_LOG(LogTemp, Warning, TEXT("WebSockets error parsing message!"));
	}
	return CarInfo;
}
