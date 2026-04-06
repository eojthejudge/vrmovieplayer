// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "VideoPlayerController.h"
#include "VideoFileBrowser.generated.h"

/**
 * An actor that displays a VR-space file browser panel for selecting video files.
 * Attach to VRPlayerPawn to toggle visibility with a controller button.
 * Interaction is handled via UWidgetInteractionComponent on the pawn's right hand.
 */
UCLASS()
class VRMOVIEPLAYER_API AVideoFileBrowser : public AActor
{
	GENERATED_BODY()

public:
	AVideoFileBrowser();

protected:
	virtual void BeginPlay() override;

public:
	// 3D widget panel displayed in world space
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "File Browser")
	UWidgetComponent* WidgetComponent;

	// Video player to receive the selected file
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "File Browser")
	AVideoPlayerController* VideoPlayerController;

	// Directory to scan for video files
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "File Browser")
	FString VideoDirectory;

	UFUNCTION(BlueprintCallable, Category = "File Browser")
	void ShowBrowser();

	UFUNCTION(BlueprintCallable, Category = "File Browser")
	void HideBrowser();

	UFUNCTION(BlueprintCallable, Category = "File Browser")
	void ToggleBrowser();

	UFUNCTION(BlueprintCallable, Category = "File Browser")
	bool IsBrowserVisible() const;

	// Re-scan the directory and rebuild the widget
	UFUNCTION(BlueprintCallable, Category = "File Browser")
	void RefreshFileList();

private:
	TArray<FString> VideoFiles;
	TSharedPtr<SWidget> FileBrowserWidget;
	bool bIsVisible;

	void BuildWidget();
	void OnFileSelected(FString FilePath);
};
