// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "IXRTrackingSystem.h"
#include "MotionControllerComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "VideoFileBrowser.h"
#include "VRPlayerPawn.generated.h"

UCLASS()
class VRMOVIEPLAYER_API AVRPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AVRPlayerPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// VR Root component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	USceneComponent* VRRoot;

	// Camera component for VR
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	UCameraComponent* Camera;

	// Left motion controller
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	UMotionControllerComponent* LeftController;

	// Right motion controller
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	UMotionControllerComponent* RightController;

	// Widget interaction component on the right hand — drives the VR file browser cursor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	UWidgetInteractionComponent* WidgetInteraction;

	// Enable or disable VR mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	bool bVREnabled;

	// Reference set by GameMode after spawning the file browser
	UPROPERTY(BlueprintReadWrite, Category = "VR")
	AVideoFileBrowser* FileBrowser;

private:
	void InitializeVR();

	// Input handlers
	void ToggleFileBrowser();
	void TriggerPressed();
	void TriggerReleased();
};
