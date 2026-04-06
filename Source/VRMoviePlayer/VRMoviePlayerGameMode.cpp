// Copyright Epic Games, Inc. All Rights Reserved.

#include "VRMoviePlayerGameMode.h"
#include "VRPlayerPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AVRMoviePlayerGameMode::AVRMoviePlayerGameMode()
{
	DefaultPawnClass = AVRPlayerPawn::StaticClass();

	APIPort = 8080;
	bAutoPlayOnStart = false;
	DefaultVideoPath = TEXT("");
	VideoDirectory = TEXT("");

	VideoPlayerController = nullptr;
	APIServer = nullptr;
	MovieScreen = nullptr;
	FileBrowser = nullptr;
}

void AVRMoviePlayerGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("VR Movie Player Game Mode Starting..."));

	InitializeVRMoviePlayer();
}

void AVRMoviePlayerGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Clean up
	if (APIServer)
	{
		APIServer->StopServer();
	}

	if (VideoPlayerController)
	{
		VideoPlayerController->StopVideo();
	}

	Super::EndPlay(EndPlayReason);
}

void AVRMoviePlayerGameMode::InitializeVRMoviePlayer()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("World is null, cannot initialize VR Movie Player"));
		return;
	}

	// Spawn Video Player Controller
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Name = FName(TEXT("VideoPlayerController"));

	VideoPlayerController = World->SpawnActor<AVideoPlayerController>(
		AVideoPlayerController::StaticClass(),
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (!VideoPlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn Video Player Controller"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Video Player Controller spawned successfully"));

	// Spawn Movie Screen
	SpawnParams.Name = FName(TEXT("MovieScreen"));
	MovieScreen = World->SpawnActor<AVRMovieScreen>(
		AVRMovieScreen::StaticClass(),
		FVector(300.0f, 0.0f, 100.0f), // Position in front of player
		FRotator::ZeroRotator,          // Rotation handled by mesh's relative rotation
		SpawnParams
	);

	if (!MovieScreen)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn Movie Screen"));
		return;
	}

	// Link movie screen to video player controller
	MovieScreen->VideoPlayerController = VideoPlayerController;
	UE_LOG(LogTemp, Warning, TEXT("MediaPlayer is: %s"), VideoPlayerController->MediaPlayer ? TEXT("valid") : TEXT("NULL"));
	MovieScreen->InitializeScreen(VideoPlayerController->MediaPlayer);

	UE_LOG(LogTemp, Log, TEXT("Movie Screen spawned successfully"));

	// Spawn API Server
	SpawnParams.Name = FName(TEXT("APIServer"));
	APIServer = World->SpawnActor<AVideoAPIServer>(
		AVideoAPIServer::StaticClass(),
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (!APIServer)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn API Server"));
		return;
	}

	// Configure API Server
	APIServer->ServerPort = APIPort;
	APIServer->VideoPlayerController = VideoPlayerController;

	UE_LOG(LogTemp, Log, TEXT("API Server spawned successfully"));

	// Spawn VR File Browser (positioned left of the video screen; hidden until player opens it)
	SpawnParams.Name = FName(TEXT("FileBrowser"));
	FileBrowser = World->SpawnActor<AVideoFileBrowser>(
		AVideoFileBrowser::StaticClass(),
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (!FileBrowser)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn Video File Browser"));
		return;
	}

	FileBrowser->VideoPlayerController = VideoPlayerController;
	if (!VideoDirectory.IsEmpty())
	{
		FileBrowser->VideoDirectory = VideoDirectory;
	}

	UE_LOG(LogTemp, Log, TEXT("Video File Browser spawned (hidden until Y button / F key)"));

	// Give the file browser reference to the pawn so it can toggle it
	APlayerController* PC = World->GetFirstPlayerController();
	if (PC)
	{
		AVRPlayerPawn* VRPawn = Cast<AVRPlayerPawn>(PC->GetPawn());
		if (VRPawn)
		{
			VRPawn->FileBrowser = FileBrowser;
			UE_LOG(LogTemp, Log, TEXT("FileBrowser wired to VRPlayerPawn"));
		}
	}

	// Auto-play video if configured
	if (bAutoPlayOnStart && !DefaultVideoPath.IsEmpty())
	{
		UE_LOG(LogTemp, Log, TEXT("Auto-playing video: %s"), *DefaultVideoPath);
		VideoPlayerController->PlayVideo(DefaultVideoPath);

		// Update screen aspect ratio after a short delay to ensure video is loaded
		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(
			TimerHandle,
			[this]()
			{
				if (MovieScreen)
				{
					MovieScreen->UpdateScreenAspectRatio();
				}
			},
			1.0f,
			false
		);
	}

	UE_LOG(LogTemp, Log, TEXT("VR Movie Player initialized successfully"));
	UE_LOG(LogTemp, Log, TEXT("REST API available at http://localhost:%d"), APIPort);
}
