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
		// Yaw 90 + Roll 90: normal=-X (facing player), correct video orientation
		ScreenMesh->SetRelativeRotation(FRotator(0.0f, 90.0f, 90.0f));
	}

	// Create media texture
	MediaTexture = CreateDefaultSubobject<UMediaTexture>(TEXT("MediaTexture"));
	MediaTexture->AutoClear = true;

	VideoPlayerController = nullptr;
	ScreenMaterial = nullptr;
}

void AVRMovieScreen::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("VRMovieScreen BeginPlay - Position: %s"), *GetActorLocation().ToString());

	// Create dynamic material for the screen
	CreateScreenMaterial();

	// If video player controller is set, initialize the screen
	if (VideoPlayerController && VideoPlayerController->MediaPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("Initializing screen with MediaPlayer"));
		InitializeScreen(VideoPlayerController->MediaPlayer);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("VideoPlayerController or MediaPlayer is null!"));
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

	// Connect media texture to media player and initialize its resource
	MediaTexture->SetMediaPlayer(MediaPlayer);
	MediaTexture->UpdateResource();

	// Log texture and material state for debugging
	UE_LOG(LogTemp, Warning, TEXT("MediaTexture: %s, ScreenMaterial: %s"),
		MediaTexture ? TEXT("valid") : TEXT("null"),
		ScreenMaterial ? TEXT("valid") : TEXT("null"));

	if (ScreenMaterial)
	{
		ScreenMaterial->SetTextureParameterValue(FName("MediaTexture"), MediaTexture);
		UE_LOG(LogTemp, Warning, TEXT("SetTextureParameterValue called for 'MediaTexture'"));
		UE_LOG(LogTemp, Log, TEXT("VR Movie Screen initialized successfully"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ScreenMaterial is null"));
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
	if (!ScreenMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("ScreenMesh is null"));
		return;
	}

	// Try to load a custom video screen material first
	UMaterial* BaseMaterial = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), nullptr, TEXT("/Game/Materials/M_VideoScreen")));
	
	// Fallback to engine material if custom doesn't exist
	if (!BaseMaterial)
	{
		UE_LOG(LogTemp, Warning, TEXT("Custom M_VideoScreen material not found. Using default material."));
		BaseMaterial = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial")));
	}

	// Create dynamic material instance
	if (BaseMaterial)
	{
		ScreenMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		if (ScreenMaterial)
		{
			ScreenMesh->SetMaterial(0, ScreenMaterial);
			UE_LOG(LogTemp, Log, TEXT("Screen material created and assigned"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Could not load any material for screen"));
	}
}
