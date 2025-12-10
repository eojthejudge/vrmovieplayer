// Copyright Epic Games, Inc. All Rights Reserved.

#include "VRMovieScreen.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "MediaPlayer.h"
#include "MediaTexture.h"

AVRMovieScreen::AVRMovieScreen()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create screen mesh component
	ScreenMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ScreenMesh"));
	RootComponent = ScreenMesh;

	// Set default dimensions (16:9 aspect ratio, 5 meters wide)
	ScreenWidth = 500.0f;  // 5 meters
	ScreenHeight = 281.25f; // Maintains 16:9 aspect ratio
	DistanceFromPlayer = 300.0f; // 3 meters away

	// Load plane mesh for the screen
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("/Engine/BasicShapes/Plane"));
	if (PlaneMesh.Succeeded())
	{
		ScreenMesh->SetStaticMesh(PlaneMesh.Object);
		ScreenMesh->SetRelativeScale3D(FVector(ScreenWidth / 100.0f, ScreenHeight / 100.0f, 1.0f));
		ScreenMesh->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f)); // Face the player
	}

	// Create media texture
	MediaTexture = CreateDefaultSubobject<UMediaTexture>(TEXT("MediaTexture"));
	MediaTexture->AutoClear = true;
	MediaTexture->SetDefaultMediaPlayer(nullptr);

	VideoPlayerController = nullptr;
	ScreenMaterial = nullptr;
}

void AVRMovieScreen::BeginPlay()
{
	Super::BeginPlay();

	// Create dynamic material for the screen
	CreateScreenMaterial();

	// If video player controller is set, initialize the screen
	if (VideoPlayerController && VideoPlayerController->MediaPlayer)
	{
		InitializeScreen(VideoPlayerController->MediaPlayer);
	}
}

void AVRMovieScreen::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AVRMovieScreen::InitializeScreen(UMediaPlayer* MediaPlayer)
{
	if (!MediaPlayer || !MediaTexture)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot initialize screen: MediaPlayer or MediaTexture is null"));
		return;
	}

	// Connect media texture to media player
	MediaTexture->SetMediaPlayer(MediaPlayer);

	// Update material to use the media texture
	if (ScreenMaterial)
	{
		ScreenMaterial->SetTextureParameterValue(FName("MediaTexture"), MediaTexture);
		UE_LOG(LogTemp, Log, TEXT("VR Movie Screen initialized successfully"));
	}
}

void AVRMovieScreen::UpdateScreenAspectRatio()
{
	if (!VideoPlayerController || !VideoPlayerController->MediaPlayer)
	{
		return;
	}

	UMediaPlayer* MediaPlayer = VideoPlayerController->MediaPlayer;
	if (!MediaPlayer->IsReady())
	{
		return;
	}

	// Get video dimensions
	FIntPoint VideoDimensions = MediaPlayer->GetVideoTrackDimensions(INDEX_NONE, INDEX_NONE);
	if (VideoDimensions.X > 0 && VideoDimensions.Y > 0)
	{
		float AspectRatio = static_cast<float>(VideoDimensions.X) / static_cast<float>(VideoDimensions.Y);

		// Adjust screen height to maintain aspect ratio
		ScreenHeight = ScreenWidth / AspectRatio;

		// Update mesh scale
		ScreenMesh->SetRelativeScale3D(FVector(ScreenWidth / 100.0f, ScreenHeight / 100.0f, 1.0f));

		UE_LOG(LogTemp, Log, TEXT("Screen aspect ratio updated: %dx%d (AR: %.2f)"),
			VideoDimensions.X, VideoDimensions.Y, AspectRatio);
	}
}

void AVRMovieScreen::SetScreenPosition(const FVector& PlayerLocation, const FRotator& PlayerRotation)
{
	// Calculate position in front of player
	FVector ForwardVector = PlayerRotation.Vector();
	FVector ScreenLocation = PlayerLocation + (ForwardVector * DistanceFromPlayer);

	// Set screen to face the player
	FRotator ScreenRotation = (PlayerLocation - ScreenLocation).Rotation();
	ScreenRotation.Pitch = 0.0f; // Keep screen level

	SetActorLocation(ScreenLocation);
	SetActorRotation(ScreenRotation);

	UE_LOG(LogTemp, Log, TEXT("Screen positioned at: %s"), *ScreenLocation.ToString());
}

void AVRMovieScreen::CreateScreenMaterial()
{
	// Try to load a basic unlit material
	static ConstructorHelpers::FObjectFinder<UMaterial> UnlitMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial"));

	UMaterial* BaseMaterial = nullptr;
	if (UnlitMaterial.Succeeded())
	{
		BaseMaterial = UnlitMaterial.Object;
	}

	// Create dynamic material instance
	if (BaseMaterial)
	{
		ScreenMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		if (ScreenMaterial)
		{
			ScreenMesh->SetMaterial(0, ScreenMaterial);
			UE_LOG(LogTemp, Log, TEXT("Screen material created"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not load base material for screen. Create a custom material with MediaTexture parameter in Unreal Editor."));
	}
}
