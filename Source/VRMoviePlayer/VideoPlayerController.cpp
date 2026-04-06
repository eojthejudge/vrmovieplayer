// Copyright Epic Games, Inc. All Rights Reserved.

#include "VideoPlayerController.h"
#include "MediaPlayer.h"
#include "MediaSoundComponent.h"
#include "FileMediaSource.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"

AVideoPlayerController::AVideoPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create MediaPlayer
	MediaPlayer = CreateDefaultSubobject<UMediaPlayer>(TEXT("MediaPlayer"));
	MediaSource = CreateDefaultSubobject<UFileMediaSource>(TEXT("MediaSource"));

	// Create sound component and bind it to the media player for audio output
	MediaSoundComponent = CreateDefaultSubobject<UMediaSoundComponent>(TEXT("MediaSoundComponent"));
	MediaSoundComponent->SetupAttachment(RootComponent);

	PlayerState = EPlayerState::Stopped;
	CurrentVideoPath = TEXT("");
}

void AVideoPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (MediaPlayer)
	{
		MediaPlayer->PlayOnOpen = true;
		MediaPlayer->SetLooping(false);
	}

	if (MediaSoundComponent)
	{
		MediaSoundComponent->SetMediaPlayer(MediaPlayer);
	}
}

void AVideoPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdatePlayerState();
}

bool AVideoPlayerController::PlayVideo(const FString& VideoFilePath)
{
	if (!MediaPlayer || !MediaSource)
	{
		UE_LOG(LogTemp, Error, TEXT("MediaPlayer or MediaSource is null"));
		return false;
	}

	// Check if file exists
	if (!FPaths::FileExists(VideoFilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("Video file does not exist: %s"), *VideoFilePath);
		return false;
	}

	CurrentVideoPath = VideoFilePath;
	MediaSource->SetFilePath(VideoFilePath);

	bool bSuccess = MediaPlayer->OpenSource(MediaSource);
	if (bSuccess)
	{
		PlayerState = EPlayerState::Playing;
		UE_LOG(LogTemp, Log, TEXT("Playing video: %s"), *VideoFilePath);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to open video: %s"), *VideoFilePath);
		PlayerState = EPlayerState::Stopped;
	}

	return bSuccess;
}

void AVideoPlayerController::PauseVideo()
{
	if (MediaPlayer && MediaPlayer->IsPlaying())
	{
		MediaPlayer->Pause();
		PlayerState = EPlayerState::Paused;
		UE_LOG(LogTemp, Log, TEXT("Video paused"));
	}
}

void AVideoPlayerController::ResumeVideo()
{
	if (MediaPlayer && MediaPlayer->IsPaused())
	{
		MediaPlayer->Play();
		PlayerState = EPlayerState::Playing;
		UE_LOG(LogTemp, Log, TEXT("Video resumed"));
	}
}

void AVideoPlayerController::StopVideo()
{
	if (MediaPlayer)
	{
		MediaPlayer->Close();
		PlayerState = EPlayerState::Stopped;
		CurrentVideoPath = TEXT("");
		UE_LOG(LogTemp, Log, TEXT("Video stopped"));
	}
}

FString AVideoPlayerController::GetVideoFilename() const
{
	if (CurrentVideoPath.IsEmpty())
	{
		return TEXT("");
	}

	return FPaths::GetCleanFilename(CurrentVideoPath);
}

FString AVideoPlayerController::GetPlayerStateString() const
{
	switch (PlayerState)
	{
	case EPlayerState::Playing:
		return TEXT("playing");
	case EPlayerState::Paused:
		return TEXT("paused");
	case EPlayerState::Stopped:
		return TEXT("stopped");
	default:
		return TEXT("unknown");
	}
}

float AVideoPlayerController::GetCurrentTimestamp() const
{
	if (MediaPlayer && MediaPlayer->IsReady())
	{
		FTimespan Time = MediaPlayer->GetTime();
		return static_cast<float>(Time.GetTotalSeconds());
	}
	return 0.0f;
}

float AVideoPlayerController::GetTotalDuration() const
{
	if (MediaPlayer && MediaPlayer->IsReady())
	{
		FTimespan Duration = MediaPlayer->GetDuration();
		return static_cast<float>(Duration.GetTotalSeconds());
	}
	return 0.0f;
}

void AVideoPlayerController::SeekToTime(float TimeInSeconds)
{
	if (MediaPlayer && MediaPlayer->IsReady())
	{
		FTimespan Time = FTimespan::FromSeconds(TimeInSeconds);
		MediaPlayer->Seek(Time);
		UE_LOG(LogTemp, Log, TEXT("Seeking to time: %f seconds"), TimeInSeconds);
	}
}

TArray<FString> AVideoPlayerController::ListVideoFiles(const FString& Directory) const
{
	TArray<FString> Result;

	if (Directory.IsEmpty() || !FPaths::DirectoryExists(Directory))
	{
		UE_LOG(LogTemp, Warning, TEXT("Video directory not found: %s"), *Directory);
		return Result;
	}

	static const TArray<FString> SupportedExtensions = { TEXT("*.mp4"), TEXT("*.mkv"), TEXT("*.avi"), TEXT("*.mov"), TEXT("*.webm") };

	IFileManager& FileManager = IFileManager::Get();
	for (const FString& Ext : SupportedExtensions)
	{
		TArray<FString> Found;
		FileManager.FindFiles(Found, *(Directory / Ext), true, false);
		for (const FString& File : Found)
		{
			Result.Add(Directory / File);
		}
	}

	Result.Sort();
	UE_LOG(LogTemp, Log, TEXT("Found %d video file(s) in %s"), Result.Num(), *Directory);
	return Result;
}

void AVideoPlayerController::UpdatePlayerState()
{
	if (!MediaPlayer)
	{
		return;
	}

	// Update state based on media player status
	if (MediaPlayer->IsPlaying())
	{
		PlayerState = EPlayerState::Playing;
	}
	else if (MediaPlayer->IsPaused())
	{
		PlayerState = EPlayerState::Paused;
	}
	else if (!MediaPlayer->IsReady() || MediaPlayer->IsClosed())
	{
		PlayerState = EPlayerState::Stopped;
	}
}
