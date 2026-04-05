// Copyright Epic Games, Inc. All Rights Reserved.

#include "VRPlayerPawn.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "IXRTrackingSystem.h"
#include "IXRCamera.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

AVRPlayerPawn::AVRPlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// Auto-possess player 0 so the camera activates on Play
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Create VR Root component
	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	RootComponent = VRRoot;

	// Create Camera component
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VRRoot);
	Camera->bUsePawnControlRotation = false;

	bVREnabled = true;
}

void AVRPlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	if (bVREnabled)
	{
		InitializeVR();
	}
}

void AVRPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AVRPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Input bindings can be added here for VR controller interactions
	// For example: pause/play video, stop video, etc.
}

void AVRPlayerPawn::InitializeVR()
{
	// Get the XR tracking system
	if (GEngine && GEngine->XRSystem.IsValid())
	{
		TSharedPtr<IXRTrackingSystem, ESPMode::ThreadSafe> XRSystem = GEngine->XRSystem;

		if (XRSystem->IsHeadTrackingAllowed())
		{
			UE_LOG(LogTemp, Log, TEXT("VR Mode enabled - HMD detected"));

			// Set tracking origin to stage level (floor-level tracking)
			XRSystem->SetTrackingOrigin(EHMDTrackingOrigin::Stage);

			// Reset orientation and position
			XRSystem->ResetOrientationAndPosition(0.0f);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("VR Mode enabled but head tracking not allowed. Running in non-VR mode."));
			bVREnabled = false;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("VR Mode enabled but no XR system detected. Running in non-VR mode."));
		bVREnabled = false;
	}
}
