// Copyright Epic Games, Inc. All Rights Reserved.

#include "VRMoviePlayer.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FVRMoviePlayerModule"

void FVRMoviePlayerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory
	UE_LOG(LogTemp, Log, TEXT("VRMoviePlayer Module Started"));
}

void FVRMoviePlayerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module
	UE_LOG(LogTemp, Log, TEXT("VRMoviePlayer Module Shutdown"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_PRIMARY_GAME_MODULE(FVRMoviePlayerModule, VRMoviePlayer, "VRMoviePlayer");
