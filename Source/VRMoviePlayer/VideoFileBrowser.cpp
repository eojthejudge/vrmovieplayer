// Copyright Epic Games, Inc. All Rights Reserved.

#include "VideoFileBrowser.h"
#include "Components/WidgetComponent.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SBoxPanel.h"
#include "Styling/CoreStyle.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/World.h"

AVideoFileBrowser::AVideoFileBrowser()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(Root);
	// Render the widget as a quad in 3D world space (required for VR visibility)
	WidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	// 600x800 pixel resolution; scale makes the quad ~60cm x 80cm in world space
	WidgetComponent->SetDrawSize(FVector2D(600.0f, 800.0f));
	WidgetComponent->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));

	VideoDirectory = TEXT("");
	bIsVisible = false;

	// Hidden until explicitly shown
	SetActorHiddenInGame(true);
}

void AVideoFileBrowser::BeginPlay()
{
	Super::BeginPlay();

	if (VideoDirectory.IsEmpty())
	{
#if PLATFORM_ANDROID
		VideoDirectory = TEXT("/sdcard/Movies");
#else
		VideoDirectory = FPaths::ProjectDir() / TEXT("Videos");
#endif
		UE_LOG(LogTemp, Log, TEXT("VideoFileBrowser: using default directory: %s"), *VideoDirectory);
	}

	RefreshFileList();
}

void AVideoFileBrowser::ShowBrowser()
{
	RefreshFileList();

	if (UWorld* World = GetWorld())
	{
		APlayerController* PC = World->GetFirstPlayerController();

		// Place the panel in front of the player camera each time it's shown
		if (PC && PC->PlayerCameraManager)
		{
			FVector CamLoc = PC->PlayerCameraManager->GetCameraLocation();
			FRotator CamRot = PC->PlayerCameraManager->GetCameraRotation();

			FRotator FaceRot = FRotator(0.0f, CamRot.Yaw, 0.0f);
			FVector NewLocation = CamLoc + FaceRot.Vector() * 150.0f;
			NewLocation.Z = CamLoc.Z;

			SetActorLocation(NewLocation);
			SetActorRotation(FRotator(0.0f, CamRot.Yaw + 180.0f, 0.0f));
		}

		// Show mouse cursor so the player can click file entries on desktop
		if (PC)
		{
			PC->bShowMouseCursor = true;
			FInputModeGameAndUI InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			InputMode.SetHideCursorDuringCapture(false);
			PC->SetInputMode(InputMode);
		}
	}

	SetActorHiddenInGame(false);
	WidgetComponent->SetVisibility(true);
	bIsVisible = true;
	UE_LOG(LogTemp, Log, TEXT("VideoFileBrowser: shown (%d files)"), VideoFiles.Num());
}

void AVideoFileBrowser::HideBrowser()
{
	SetActorHiddenInGame(true);
	WidgetComponent->SetVisibility(false);
	bIsVisible = false;

	// Restore game-only input and hide cursor
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			PC->bShowMouseCursor = false;
			PC->SetInputMode(FInputModeGameOnly());
		}
	}

	UE_LOG(LogTemp, Log, TEXT("VideoFileBrowser: hidden"));
}

void AVideoFileBrowser::ToggleBrowser()
{
	if (bIsVisible)
		HideBrowser();
	else
		ShowBrowser();
}

bool AVideoFileBrowser::IsBrowserVisible() const
{
	return bIsVisible;
}

void AVideoFileBrowser::RefreshFileList()
{
	VideoFiles.Empty();

	if (VideoPlayerController)
	{
		VideoFiles = VideoPlayerController->ListVideoFiles(VideoDirectory);
	}
	else
	{
		// Scan directly if no controller reference yet
		if (!VideoDirectory.IsEmpty() && FPaths::DirectoryExists(VideoDirectory))
		{
			static const TArray<FString> Exts = { TEXT("*.mp4"), TEXT("*.mkv"), TEXT("*.avi"), TEXT("*.mov"), TEXT("*.webm") };
			IFileManager& FM = IFileManager::Get();
			for (const FString& Ext : Exts)
			{
				TArray<FString> Found;
				FM.FindFiles(Found, *(VideoDirectory / Ext), true, false);
				for (const FString& File : Found)
					VideoFiles.Add(VideoDirectory / File);
			}
			VideoFiles.Sort();
		}
	}

	BuildWidget();
}

void AVideoFileBrowser::BuildWidget()
{
	TSharedRef<SScrollBox> ScrollBox = SNew(SScrollBox);

	if (VideoFiles.IsEmpty())
	{
		ScrollBox->AddSlot()
		.Padding(8.0f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("No video files found.\nPlace videos in:\n") + VideoDirectory))
			.ColorAndOpacity(FLinearColor(0.8f, 0.8f, 0.8f, 1.0f))
		];
	}
	else
	{
		for (const FString& FilePath : VideoFiles)
		{
			const FString FileName = FPaths::GetCleanFilename(FilePath);
			TWeakObjectPtr<AVideoFileBrowser> WeakSelf = this;
			FString FilePathCopy = FilePath;

			ScrollBox->AddSlot()
			.Padding(4.0f, 3.0f)
			[
				SNew(SButton)
				.ButtonColorAndOpacity(FLinearColor(0.15f, 0.15f, 0.35f, 1.0f))
				.OnClicked_Lambda([WeakSelf, FilePathCopy]() -> FReply
				{
					if (WeakSelf.IsValid())
						WeakSelf->OnFileSelected(FilePathCopy);
					return FReply::Handled();
				})
				[
					SNew(SBox)
					.Padding(FMargin(8.0f, 6.0f))
					[
						SNew(STextBlock)
						.Text(FText::FromString(FileName))
						.ColorAndOpacity(FLinearColor::White)
					]
				]
			];
		}
	}

	FileBrowserWidget =
		SNew(SBorder)
		.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
		.BorderBackgroundColor(FLinearColor(0.05f, 0.05f, 0.12f, 0.97f))
		.Padding(12.0f)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 0.0f, 0.0f, 10.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Select Video File")))
				.ColorAndOpacity(FLinearColor(0.9f, 0.9f, 1.0f, 1.0f))
			]
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				ScrollBox
			]
		];

	WidgetComponent->SetSlateWidget(FileBrowserWidget);
}

void AVideoFileBrowser::OnFileSelected(FString FilePath)
{
	UE_LOG(LogTemp, Log, TEXT("VideoFileBrowser: selected '%s'"), *FilePath);

	if (VideoPlayerController)
	{
		VideoPlayerController->PlayVideo(FilePath);
	}

	HideBrowser();
}
