// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VideoPlayerController.h"
#include "VideoAPIServer.h"
#include "VRMovieScreen.h"
#include "VideoFileBrowser.h"
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

	// VR file browser instance
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR Movie Player")
	AVideoFileBrowser* FileBrowser;

	// Default video file path for testing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR Movie Player", meta = (DisplayName = "Default Video Path"))
	FString DefaultVideoPath;

	// Directory to browse for video files (defaults to platform-appropriate path if empty)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR Movie Player", meta = (DisplayName = "Video Directory"))
	FString VideoDirectory;

	// API server port
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR Movie Player", meta = (DisplayName = "API Port", UIMin = 1024, UIMax = 65535))
	int32 APIPort;

	// Auto-play video on start
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR Movie Player", meta = (DisplayName = "Auto Play on Start"))
	bool bAutoPlayOnStart;

private:
	void InitializeVRMoviePlayer();
};
