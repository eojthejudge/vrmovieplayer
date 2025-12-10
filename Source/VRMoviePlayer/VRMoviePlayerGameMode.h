// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VideoPlayerController.h"
#include "VideoAPIServer.h"
#include "VRMovieScreen.h"
#include "VRMoviePlayerGameMode.generated.h"

UCLASS()
class VRMOVIEPLAYER_API AVRMoviePlayerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AVRMoviePlayerGameMode();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Video player controller instance
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR Movie Player")
	AVideoPlayerController* VideoPlayerController;

	// API server instance
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR Movie Player")
	AVideoAPIServer* APIServer;

	// VR screen instance
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR Movie Player")
	AVRMovieScreen* MovieScreen;

	// Default video file path for testing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR Movie Player")
	FString DefaultVideoPath;

	// API server port
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR Movie Player")
	int32 APIPort;

	// Auto-play video on start
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR Movie Player")
	bool bAutoPlayOnStart;

private:
	void InitializeVRMoviePlayer();
};
