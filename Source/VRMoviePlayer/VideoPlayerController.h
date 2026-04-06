// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MediaPlayer.h"
#include "MediaSoundComponent.h"
#include "MediaSource.h"
#include "FileMediaSource.h"
#include "VideoPlayerController.generated.h"

UENUM(BlueprintType)
enum class EPlayerState : uint8
{
	Stopped UMETA(DisplayName = "Stopped"),
	Playing UMETA(DisplayName = "Playing"),
	Paused UMETA(DisplayName = "Paused")
};

UCLASS()
class VRMOVIEPLAYER_API AVideoPlayerController : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVideoPlayerController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Media Player component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Video")
	UMediaPlayer* MediaPlayer;

	// Audio output component — routes audio from MediaPlayer to the speakers
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Video")
	UMediaSoundComponent* MediaSoundComponent;

	// Current media source
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Video")
	UFileMediaSource* MediaSource;

	// Current video file path
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Video")
	FString CurrentVideoPath;

	// Current player state
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Video")
	EPlayerState PlayerState;

	// Play video from file path
	UFUNCTION(BlueprintCallable, Category = "Video")
	bool PlayVideo(const FString& VideoFilePath);

	// Pause video playback
	UFUNCTION(BlueprintCallable, Category = "Video")
	void PauseVideo();

	// Resume video playback
	UFUNCTION(BlueprintCallable, Category = "Video")
	void ResumeVideo();

	// Stop video playback
	UFUNCTION(BlueprintCallable, Category = "Video")
	void StopVideo();

	// Get current video filename
	UFUNCTION(BlueprintCallable, Category = "Video")
	FString GetVideoFilename() const;

	// Get player state as string
	UFUNCTION(BlueprintCallable, Category = "Video")
	FString GetPlayerStateString() const;

	// Get current timestamp in seconds
	UFUNCTION(BlueprintCallable, Category = "Video")
	float GetCurrentTimestamp() const;

	// Get total video duration in seconds
	UFUNCTION(BlueprintCallable, Category = "Video")
	float GetTotalDuration() const;

	// Seek to specific time in seconds
	UFUNCTION(BlueprintCallable, Category = "Video")
	void SeekToTime(float TimeInSeconds);

	// List video files in a directory (returns full paths)
	UFUNCTION(BlueprintCallable, Category = "Video")
	TArray<FString> ListVideoFiles(const FString& Directory) const;

private:
	void UpdatePlayerState();
};
