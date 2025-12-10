// Copyright Epic Games, Inc. All Rights Reserved.

#include "VRPlayerPawn.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "Kismet/GameplayStatics.h"

AVRPlayerPawn::AVRPlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create VR Root component
	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	RootComponent = VRRoot;

	// Create Camera component
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VRRoot);
	Camera->bUsePawnControlRotation = false;

	// Create Left Motion Controller
	LeftController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftController"));
	LeftController->SetupAttachment(VRRoot);
	LeftController->MotionSource = FName("Left");

	// Create Right Motion Controller
	RightController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightController"));
	RightController->SetupAttachment(VRRoot);
	RightController->MotionSource = FName("Right");

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
	// Enable HMD
	UHeadMountedDisplayFunctionLibrary::EnableHMD(true);

	// Check if HMD is connected
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		UE_LOG(LogTemp, Log, TEXT("VR Mode enabled - HMD detected"));

		// Set tracking origin to floor level
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);

		// Reset orientation and position
		UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(0.0f, EOrientPositionSelector::OrientationAndPosition);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("VR Mode enabled but no HMD detected. Running in non-VR mode."));
		bVREnabled = false;
	}
}
