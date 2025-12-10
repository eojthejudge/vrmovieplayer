// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "MediaPlayer.h"
#include "MediaTexture.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "VideoPlayerController.h"
#include "VRMovieScreen.generated.h"

UCLASS()
class VRMOVIEPLAYER_API AVRMovieScreen : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVRMovieScreen();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Screen mesh component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Screen")
	UStaticMeshComponent* ScreenMesh;

	// Media texture for displaying video
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Screen")
	UMediaTexture* MediaTexture;

	// Dynamic material instance for the screen
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Screen")
	UMaterialInstanceDynamic* ScreenMaterial;

	// Reference to video player controller
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screen")
	AVideoPlayerController* VideoPlayerController;

	// Screen width in Unreal units
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screen")
	float ScreenWidth;

	// Screen height in Unreal units
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screen")
	float ScreenHeight;

	// Distance from player spawn point
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screen")
	float DistanceFromPlayer;

	// Initialize screen with media player
	UFUNCTION(BlueprintCallable, Category = "Screen")
	void InitializeScreen(UMediaPlayer* MediaPlayer);

	// Update screen size based on video aspect ratio
	UFUNCTION(BlueprintCallable, Category = "Screen")
	void UpdateScreenAspectRatio();

	// Set screen position relative to player
	UFUNCTION(BlueprintCallable, Category = "Screen")
	void SetScreenPosition(const FVector& PlayerLocation, const FRotator& PlayerRotation);

private:
	void CreateScreenMaterial();
};
