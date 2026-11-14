// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "VRMovieScreen.h"
#include "VRPlayerPawn.h"
#include "Engine/World.h"
#include "Misc/AutomationTest.h"
#include "Misc/CoreDelegates.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMovieScreenRecenterTest, "VRMoviePlayer.Screen.Recenter",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMovieScreenRecenterTest::RunTest(const FString& Parameters)
{
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	if (!TestNotNull(TEXT("Test world"), World))
	{
		return false;
	}

	AVRMovieScreen* Screen = World->SpawnActor<AVRMovieScreen>();
	if (TestNotNull(TEXT("Movie screen"), Screen))
	{
		const FVector CameraLocation(20.0f, -30.0f, 160.0f);
		const FRotator Views[] = {
			FRotator(0.0f, 0.0f, 0.0f),
			FRotator(45.0f, 90.0f, 20.0f),
			FRotator(-45.0f, -90.0f, -20.0f)
		};

		for (const FRotator& View : Views)
		{
			Screen->SetScreenPosition(CameraLocation, View);
			const FVector Forward = View.Vector();
			TestTrue(TEXT("Screen center follows the camera view"),
				Screen->GetActorLocation().Equals(CameraLocation + Forward * Screen->DistanceFromPlayer, 0.01f));
			TestTrue(TEXT("Screen faces the camera"),
				Screen->GetActorQuat().RotateVector(FVector::UpVector).Equals(-Forward, 0.001f));
			TestTrue(TEXT("Screen remains upright without following head roll"),
				Screen->GetActorQuat().RotateVector(FVector::ForwardVector).Equals(
					FRotator(0.0f, View.Yaw, 0.0f).RotateVector(FVector::RightVector), 0.001f));
		}
	}

	World->DestroyWorld(false);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMovieControllerRecenterTest, "VRMoviePlayer.Screen.ControllerRecenter",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMovieControllerRecenterTest::RunTest(const FString& Parameters)
{
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	if (!TestNotNull(TEXT("Test world"), World))
	{
		return false;
	}

	AVRMovieScreen* Screen = World->SpawnActor<AVRMovieScreen>();
	AVRPlayerPawn* Pawn = World->SpawnActor<AVRPlayerPawn>();
	if (TestNotNull(TEXT("Movie screen"), Screen) && TestNotNull(TEXT("Player pawn"), Pawn))
	{
		Pawn->MovieScreen = Screen;
		Pawn->DispatchBeginPlay();
		const FVector CameraLocation(20.0f, -30.0f, 160.0f);
		const FRotator CameraRotation(45.0f, 90.0f, 0.0f);
		Pawn->Camera->SetWorldLocationAndRotation(CameraLocation, CameraRotation);

		const FVector OriginalLocation = Screen->GetActorLocation();
		FCoreDelegates::VRControllerRecentered.Broadcast();
		Pawn->Tick(0.0f);
		TestTrue(TEXT("Recenter waits for the camera update"), Screen->GetActorLocation().Equals(OriginalLocation));
		Pawn->Tick(0.0f);
		TestTrue(TEXT("Controller recenter moves the screen to the camera view"),
			Screen->GetActorLocation().Equals(CameraLocation + CameraRotation.Vector() * Screen->DistanceFromPlayer, 0.01f));

		Pawn->Destroy();
		TestFalse(TEXT("Controller recenter handler is removed at end play"),
			FCoreDelegates::VRControllerRecentered.IsBoundToObject(Pawn));
	}

	World->DestroyWorld(false);
	return true;
}

#endif
