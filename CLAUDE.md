# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

VR Movie Player is an Unreal Engine 5.7 application for Oculus Quest that enables VR video playback with REST API control. The project uses C++ for core functionality and targets Android (Quest) as the primary platform.

## Build System

### Building in Visual Studio

1. Generate project files by right-clicking `VRMoviePlayer.uproject` → "Generate Visual Studio project files"
2. Open `VRMoviePlayer.sln` in Visual Studio
3. Build with Development or Shipping configuration
4. Open in Unreal Editor by double-clicking `VRMoviePlayer.uproject`

### Building for Oculus Quest (Android)

- In Unreal Editor: File → Package Project → Android → Android (ASTC)
- Ensure Android SDK/NDK paths are configured in Project Settings
- Target platform is Android only (see `VRMoviePlayer.uproject`)

### Module Dependencies

The project uses these key Unreal modules (see `VRMoviePlayer.Build.cs`):
- **Media Framework**: MediaAssets, MediaUtils for video playback
- **Networking**: HTTPServer, HTTP, Sockets, Networking for REST API
- **VR**: XRBase, OculusXRHMD, OculusXRInput (Android only)
- **Data**: Json, JsonUtilities for API responses

## Architecture

### Component Orchestration

The `AVRMoviePlayerGameMode` is the central orchestrator that spawns and connects all components in `BeginPlay()`:

1. **VideoPlayerController** - Manages video playback via Unreal's Media Framework
2. **VRMovieScreen** - Renders video to a virtual screen in VR space, positioned at (300, 0, 100) facing the player
3. **VideoAPIServer** - HTTP server exposing REST endpoints on port 8080 (default)
4. **VRPlayerPawn** - VR pawn with camera and motion controllers (default pawn class)

These components are spawned as actors and linked together through direct references stored in the GameMode.

### Key Linking Pattern

The GameMode establishes relationships:
- `MovieScreen->VideoPlayerController = VideoPlayerController`
- `MovieScreen->InitializeScreen(VideoPlayerController->MediaPlayer)`
- `APIServer->VideoPlayerController = VideoPlayerController`

This coupling allows the screen to render video from the player, and the API to query/control the player.

### VR Initialization

VR setup happens in `AVRPlayerPawn::InitializeVR()` at `BeginPlay()`:
- Checks for valid XRSystem via `GEngine->XRSystem`
- Sets tracking origin to floor level
- Resets orientation/position
- Falls back to non-VR mode if HMD not detected

### Configuration

GameMode properties (editable in Blueprint/Editor):
- `DefaultVideoPath` - Path to auto-play video
- `APIPort` - REST API port (default: 8080)
- `bAutoPlayOnStart` - Whether to auto-play on startup

## REST API

Server runs on port 8080 by default. Endpoints are read-only (GET):
- `/api/filename` - Current video filename
- `/api/state` - Player state (playing/paused/stopped)
- `/api/timestamp` - Current playback position in seconds
- `/api/duration` - Total video duration in seconds

All responses return JSON. The API server is implemented using Unreal's HTTPServer module.

## Platform-Specific Code

The codebase uses platform conditionals for Quest-specific features:
```cpp
if (Target.Platform == UnrealTargetPlatform.Android)
{
    // OculusXR modules added only for Android builds
}
```

When adding VR features, check if they need Android-specific Oculus modules or can use cross-platform XRBase.

## Engine Version

Project uses Unreal Engine 5.7 with:
- `BuildSettingsVersion.V5`
- `EngineIncludeOrderVersion.Unreal5_5`

Required plugins:
- MetaXR (Oculus Quest support)
- MediaPlayerEditor
- HTTPServer
- VisualStudioTools (Win64 only)

## File Organization

All C++ source is in `Source/VRMoviePlayer/`:
- `VRMoviePlayer.h/.cpp` - Main module initialization
- `VRMoviePlayerGameMode.h/.cpp` - Central orchestrator
- `VideoPlayerController.h/.cpp` - Video playback logic
- `VideoAPIServer.h/.cpp` - HTTP REST API server
- `VRMovieScreen.h/.cpp` - Virtual screen rendering with aspect ratio handling
- `VRPlayerPawn.h/.cpp` - VR camera and motion controller setup

The `*.Build.cs` and `*.Target.cs` files define module dependencies and build targets.

## Common Development Workflow

1. Make C++ changes in Visual Studio
2. Build solution (Ctrl+Shift+B)
3. Launch Unreal Editor from the built executable or by opening `.uproject`
4. Test in VR Preview mode or package for Quest
5. For API testing, use curl or similar tool against `http://localhost:8080/api/*`

## Important Notes

- The project spawns actors dynamically at runtime; there are no pre-placed actors in the level
- Screen position (300 units forward) is hardcoded in `VRMoviePlayerGameMode.cpp:81`
- Video aspect ratio updates happen 1 second after playback starts (timer-based)
- All logging uses `UE_LOG(LogTemp, ...)` for debugging
