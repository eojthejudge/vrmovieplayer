# Copilot Instructions for VR Movie Player

## Project Overview

VR Movie Player is an Unreal Engine 5.7 application for Oculus Quest that plays video files on a virtual screen in VR, with REST API control via HTTP. The project is built in C++ and targets Android (Quest) as the primary platform.

## Build & Compile

### Visual Studio Build
1. Right-click `VRMoviePlayer.uproject` → "Generate Visual Studio project files"
2. Open `VRMoviePlayer.sln` in Visual Studio
3. Build using `Ctrl+Shift+B` (Development or Shipping configuration)
4. Open in Unreal Editor: Double-click `VRMoviePlayer.uproject`

### Command-Line Build
```powershell
# Generate Visual Studio project files
& "C:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\GenerateProjectFiles.bat" "F:\devel\git\vrmovieplayer\VRMoviePlayer.uproject"

# Build from VS solution
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" VRMoviePlayer.sln /p:Configuration=Development /p:Platform=Win64
```

### Android (Quest) Packaging
- In Unreal Editor: **File → Package Project → Android → Android (ASTC)**
- Requires Android SDK/NDK configured in Project Settings
- MetaXR plugin handles Oculus-specific features

## Architecture

### Component Orchestration
The `AVRMoviePlayerGameMode` is the central hub that spawns and wires all components in `BeginPlay()`:

1. **VideoPlayerController** — Manages video playback via Unreal Media Framework (play, pause, stop, seek)
2. **VRMovieScreen** — Virtual screen actor positioned at (300, 0, 100) facing the player; handles aspect ratio updates
3. **VideoAPIServer** — HTTP server exposing read-only GET endpoints on port 8080
4. **VRPlayerPawn** — VR pawn with camera, tracking origin set to floor level

### Key Wiring Pattern
```cpp
MovieScreen->VideoPlayerController = VideoPlayerController;
MovieScreen->InitializeScreen(VideoPlayerController->MediaPlayer);
APIServer->VideoPlayerController = VideoPlayerController;
```

No pre-placed actors exist in the level; everything is spawned dynamically at runtime.

### VR Initialization
`AVRPlayerPawn::InitializeVR()` runs at `BeginPlay()`:
- Checks for valid XRSystem via `GEngine->XRSystem`
- Sets tracking to floor level for standing VR
- Falls back to non-VR mode if HMD not detected (allows testing on desktop)

## Module Dependencies

Defined in `VRMoviePlayer.Build.cs`:
- **Media Framework**: MediaAssets, MediaUtils
- **Networking**: HTTP, HTTPServer, Sockets, Networking
- **Data**: Json, JsonUtilities
- **VR**: XRBase (all platforms), OculusXRHMD/OculusXRInput (Android only)

Android-specific dependencies are wrapped in `if (Target.Platform == UnrealTargetPlatform.Android)` blocks.

## REST API

Server listens on port 8080 (configurable). All endpoints are GET and return JSON:

- `/api/filename` — Current video filename
- `/api/state` — Player state (playing/paused/stopped)
- `/api/timestamp` — Current playback position (seconds)
- `/api/duration` — Total video duration (seconds)

Test locally with `curl http://localhost:8080/api/state`.

## Configuration

Edit via Blueprint or Game Mode properties in Unreal Editor:
- `DefaultVideoPath` — Video to auto-play on startup
- `APIPort` — REST API port (default: 8080)
- `bAutoPlayOnStart` — Enable/disable auto-play

## Key Implementation Details

- **Aspect Ratio Updates**: Triggered 1 second after playback starts (timer-based in VRMovieScreen)
- **Video Formats**: Standard formats (MP4, AVI, MKV) via Unreal's Media Framework
- **Screen Position**: Hardcoded at (300 units forward, 0 side, 100 units up)
- **Logging**: All debug logs use `UE_LOG(LogTemp, ...)` — search this macro to find debug points
- **Platform Conditionals**: Always check `#if PLATFORM_ANDROID` when using OculusXR modules

## Engine & Plugin Configuration

- **Engine Version**: 5.7 (BuildSettingsVersion.V6, EngineIncludeOrderVersion.Unreal5_7)
- **Target Platforms**: Android only (see `VRMoviePlayer.uproject`)
- **Required Plugins**:
  - MetaXR (Oculus Quest support)
  - XRBase (cross-platform VR)
  - MediaPlayerEditor
  - HTTPServer

## Code Style & Patterns

- C++ naming: `FVector`, `AActorClass`, `UComponentClass` (Unreal conventions)
- Components are spawned with `GetWorld()->SpawnActor<>()` in GameMode
- API responses use `JsonUtilities` to create `FJsonObject` then `FJsonSerializer::Stringify()`
- All actors store references in the GameMode for inter-component communication

## Common Workflows

1. **Add a new REST endpoint**: Edit `VideoAPIServer.h/.cpp`, add handler method, register route
2. **Adjust screen position/size**: Modify `VRMovieScreen.cpp` constructor and `BeginPlay()`
3. **Add video controls**: Extend `VideoPlayerController` with new playback methods
4. **Test locally**: Build → Open Unreal → Play in VR Preview or run `.uproject`
5. **Test on Quest**: Package as Android (ASTC) → Install APK → Test with curl from same network

## Gotchas

- Screen position (300, 0, 100) is hardcoded; use the GameMode property to adjust if dynamic positioning is needed
- VR only initializes if XRSystem is valid; desktop testing falls back gracefully
- API responses don't include error states — only valid states are returned (no HTTP error codes for player errors)
- MetaXR plugin must be enabled in `.uproject` for Quest builds; XRBase alone handles desktop VR

## Debugging Tips

- **Video not playing?** Check `DefaultVideoPath` format and that the file exists; look for `UE_LOG` messages in Output Log
- **API not responding?** Ensure Quest and client are on same network; check firewall; verify port 8080 isn't blocked
- **Aspect ratio wrong?** The timer in `VRMovieScreen` triggers 1 second after play—check if video duration is being detected
- **Crashes on startup?** Likely XRSystem initialization failure; check Oculus plugin is enabled for the target platform

## Related Documentation

- `CLAUDE.md` — Detailed architecture and implementation notes
- `README.md` — User-facing features and API usage
- `requirements.md` — Functional and non-functional requirements
- `vision.md` — Project goals and future enhancements
