// Copyright Epic Games, Inc. All Rights Reserved.

#include "VRPlayerPawn.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
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

	ControllerPointer = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ControllerPointer"));
	ControllerPointer->SetupAttachment(WidgetInteraction);
	ControllerPointer->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ControllerPointer->SetCastShadow(false);
	ControllerPointer->SetVisibility(false);
	ControllerPointer->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PointerMesh(TEXT("/Engine/BasicShapes/Cylinder"));
	if (PointerMesh.Succeeded())
	{
		ControllerPointer->SetStaticMesh(PointerMesh.Object);
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> PointerMaterial(TEXT("/Game/Quest1/M_ControllerPointer"));
	if (PointerMaterial.Succeeded())
	{
		ControllerPointer->SetMaterial(0, PointerMaterial.Object);
	}

	bVREnabled = true;
	FileBrowser = nullptr;
	VideoPlayerController = nullptr;
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

	const bool bShowPointer = bVREnabled && FileBrowser && FileBrowser->IsBrowserVisible() && RightController->IsTracked();
	ControllerPointer->SetVisibility(bShowPointer);
	if (bShowPointer)
	{
		const FHitResult& Hit = WidgetInteraction->GetLastHitResult();
		const float Length = Hit.bBlockingHit ? Hit.Distance : WidgetInteraction->InteractionDistance;
		ControllerPointer->SetRelativeLocation(FVector(Length * 0.5f, 0.0f, 0.0f));
		ControllerPointer->SetRelativeScale3D(FVector(0.005f, 0.005f, Length / 100.0f));
	}
}

void AVRPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("ToggleFileBrowser", IE_Pressed, this, &AVRPlayerPawn::ToggleFileBrowser);
	PlayerInputComponent->BindAction("TriggerPress", IE_Pressed, this, &AVRPlayerPawn::TriggerPressed);
	PlayerInputComponent->BindAction("TriggerPress", IE_Released, this, &AVRPlayerPawn::TriggerReleased);
	PlayerInputComponent->BindAction("RightTrigger", IE_Pressed, this, &AVRPlayerPawn::RightTriggerPressed);
	PlayerInputComponent->BindAction("RightTrigger", IE_Released, this, &AVRPlayerPawn::TriggerReleased);
	PlayerInputComponent->BindAction("TogglePlayback", IE_Pressed, this, &AVRPlayerPawn::TogglePlayback);
	PlayerInputComponent->BindAction("SkipBackward", IE_Pressed, this, &AVRPlayerPawn::SkipBackward);
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

void AVRPlayerPawn::RightTriggerPressed()
{
	if (FileBrowser && FileBrowser->IsBrowserVisible())
	{
		TriggerPressed();
	}
	else
	{
		SkipVideo(30.0f);
	}
}

void AVRPlayerPawn::TogglePlayback()
{
	if (VideoPlayerController && VideoPlayerController->MediaPlayer)
	{
		if (VideoPlayerController->MediaPlayer->IsPlaying())
		{
			VideoPlayerController->PauseVideo();
		}
		else if (VideoPlayerController->MediaPlayer->IsPaused())
		{
			VideoPlayerController->ResumeVideo();
		}
	}
}

void AVRPlayerPawn::SkipBackward()
{
	SkipVideo(-30.0f);
}

void AVRPlayerPawn::SkipVideo(float OffsetSeconds)
{
	if (VideoPlayerController && VideoPlayerController->MediaPlayer && VideoPlayerController->MediaPlayer->IsReady())
	{
		const float Duration = VideoPlayerController->GetTotalDuration();
		if (Duration > 0.0f)
		{
			const float Target = FMath::Clamp(VideoPlayerController->GetCurrentTimestamp() + OffsetSeconds, 0.0f, Duration);
			VideoPlayerController->SeekToTime(Target);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Cannot skip video: duration is unavailable"));
		}
	}
}

