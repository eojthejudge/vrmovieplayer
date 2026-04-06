// Copyright Epic Games, Inc. All Rights Reserved.

#include "VRPlayerPawn.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "IXRTrackingSystem.h"
#include "IXRCamera.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h"
#include "Components/WidgetInteractionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

AVRPlayerPawn::AVRPlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	RootComponent = VRRoot;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VRRoot);
	Camera->bUsePawnControlRotation = false;

	// Left hand motion controller
	LeftController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftController"));
	LeftController->SetupAttachment(VRRoot);
	LeftController->MotionSource = FName("Left");

	// Right hand motion controller — the widget interaction points forward from here
	RightController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightController"));
	RightController->SetupAttachment(VRRoot);
	RightController->MotionSource = FName("Right");

	// Attach the widget interaction pointer to the right hand controller
	WidgetInteraction = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("WidgetInteraction"));
	WidgetInteraction->SetupAttachment(RightController);
	WidgetInteraction->InteractionDistance = 500.0f;
	WidgetInteraction->InteractionSource = EWidgetInteractionSource::World;

	bVREnabled = true;
	FileBrowser = nullptr;
}

void AVRPlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	InitializeVR();

	// If VR didn't initialise (desktop/editor), switch interaction to mouse source
	if (!bVREnabled)
	{
		WidgetInteraction->InteractionSource = EWidgetInteractionSource::Mouse;
	}
}

void AVRPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AVRPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Y button (left controller) — toggle the VR file browser
#if PLATFORM_ANDROID
	PlayerInputComponent->BindKey(EKeys::OculusTouch_Left_FaceButton2, IE_Pressed, this, &AVRPlayerPawn::ToggleFileBrowser);
#endif
	// Keyboard fallback for desktop testing
	PlayerInputComponent->BindKey(EKeys::F, IE_Pressed, this, &AVRPlayerPawn::ToggleFileBrowser);

	// Right trigger — send pointer press/release to the widget interaction component
#if PLATFORM_ANDROID
	PlayerInputComponent->BindKey(EKeys::OculusTouch_Right_Trigger_Click, IE_Pressed, this, &AVRPlayerPawn::TriggerPressed);
	PlayerInputComponent->BindKey(EKeys::OculusTouch_Right_Trigger_Click, IE_Released, this, &AVRPlayerPawn::TriggerReleased);
#endif
	// Mouse fallback for desktop testing
	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AVRPlayerPawn::TriggerPressed);
	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &AVRPlayerPawn::TriggerReleased);
}

void AVRPlayerPawn::InitializeVR()
{
	if (GEngine && GEngine->XRSystem.IsValid())
	{
		TSharedPtr<IXRTrackingSystem, ESPMode::ThreadSafe> XRSystem = GEngine->XRSystem;

		if (XRSystem->IsHeadTrackingAllowed())
		{
			UE_LOG(LogTemp, Log, TEXT("VR Mode enabled - HMD detected"));
			XRSystem->SetTrackingOrigin(EHMDTrackingOrigin::Stage);
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

void AVRPlayerPawn::ToggleFileBrowser()
{
	if (FileBrowser)
	{
		FileBrowser->ToggleBrowser();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("VRPlayerPawn: ToggleFileBrowser called but FileBrowser is null"));
	}
}

void AVRPlayerPawn::TriggerPressed()
{
	if (WidgetInteraction)
	{
		WidgetInteraction->PressPointerKey(EKeys::LeftMouseButton);
	}
}

void AVRPlayerPawn::TriggerReleased()
{
	if (WidgetInteraction)
	{
		WidgetInteraction->ReleasePointerKey(EKeys::LeftMouseButton);
	}
}

