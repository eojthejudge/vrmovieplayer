# VR Movie Player

A VR movie player application for Oculus Quest built with Unreal Engine 5. This application allows you to watch videos on a virtual screen in VR and control playback through a REST API.

## Features

- **VR Video Playback**: Watch videos on a large virtual screen in VR
- **REST API**: Control and query the player remotely via HTTP endpoints
- **Oculus Quest Support**: Optimized for Oculus Quest platform
- **Automatic Aspect Ratio**: Screen automatically adjusts to match video dimensions
- **Multiple Video Formats**: Supports standard video formats (MP4, AVI, MKV, etc.)

## Project Structure

```
VRMoviePlayer/
├── Source/
│   └── VRMoviePlayer/
│       ├── VRMoviePlayer.h/.cpp           # Main module
│       ├── VRMoviePlayer.Build.cs         # Build configuration
│       ├── VideoPlayerController.h/.cpp   # Video playback controller
│       ├── VideoAPIServer.h/.cpp          # REST API server
│       ├── VRMovieScreen.h/.cpp          # Virtual screen actor
│       ├── VRMoviePlayerGameMode.h/.cpp  # Game mode orchestrator
│       └── VRPlayerPawn.h/.cpp           # VR player pawn
├── Config/
│   ├── DefaultEngine.ini                  # Engine configuration
│   └── DefaultGame.ini                    # Game configuration
├── Content/                               # Unreal Engine assets
├── VRMoviePlayer.uproject                # Project file
├── requirements.md                        # Project requirements
└── vision.md                             # Project vision

```

## Requirements

See [requirements.md](requirements.md) for detailed functional and non-functional requirements.

## Prerequisites

- Unreal Engine 5.0 or later
- Oculus Quest device
- Android development tools (for Quest deployment)
- Visual Studio 2019/2022 (Windows) or Xcode (Mac)

## Building the Project

### Prerequisites
- Unreal Engine 5.7
- Visual Studio 2022 with C++ build tools
- Project file paths configured for your installation

### Quick Start (Recommended)

The easiest way to build is to open the project file directly, which auto-generates and builds:

```powershell
# Adjust paths to match your UE and Visual Studio installation
& "G:\EpicLauncher\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe" "F:\devel\git\vrmovieplayer\VRMoviePlayer.uproject"
```

### Full Build Process (for troubleshooting)

If you encounter build errors, follow these steps:

```powershell
# 1. Close any running Unreal processes
Get-Process | Where-Object {$_.Name -like "*Unreal*" -or $_.Name -like "*msbuild*"} | Stop-Process -Force -ErrorAction SilentlyContinue

# 2. Navigate to project directory
cd F:\devel\git\vrmovieplayer

# 3. Clean build artifacts
Remove-Item -Path "Binaries", "Intermediate", "Saved" -Recurse -Force -ErrorAction SilentlyContinue

# 4. Regenerate Visual Studio project files
& "G:\EpicLauncher\UE_5.7\Engine\Binaries\Win64\UnrealVersionSelector.exe" /projectfiles "F:\devel\git\vrmovieplayer\VRMoviePlayer.uproject"

Start-Sleep -Seconds 3

# 5. Open Visual Studio
$vs = "G:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe"
& $vs "VRMoviePlayer.sln"
```

In Visual Studio:
- Select **Development Editor** configuration (top toolbar dropdown)
- Build: `Ctrl+Shift+B` or **Build → Build Solution**
- Wait for "Build succeeded" message
- Close Visual Studio

```powershell
# 6. Open the project in Unreal Editor
& "G:\EpicLauncher\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe" "F:\devel\git\vrmovieplayer\VRMoviePlayer.uproject"
```

### Package for Oculus Quest

Once the editor opens successfully:

1. In Unreal Editor: **File → Package Project → Android → Android (ASTC)**
2. Choose output folder
3. Wait for packaging to complete
4. Install on Quest: `adb install -r output.apk`

## REST API Endpoints

The application exposes the following REST API endpoints (default port: 8080):

### GET /api/filename
Returns the current video filename.

**Response:**
```json
{
  "filename": "example_video.mp4"
}
```

### GET /api/state
Returns the current player state.

**Response:**
```json
{
  "state": "playing"
}
```

Possible states: `playing`, `paused`, `stopped`

### GET /api/timestamp
Returns the current playback timestamp in seconds.

**Response:**
```json
{
  "timestamp": 125.50
}
```

### GET /api/duration
Returns the total video duration in seconds.

**Response:**
```json
{
  "duration": 300.00
}
```

## Configuration

You can configure the following settings in the Game Mode:

- **DefaultVideoPath**: Path to a video file to auto-play on start
- **APIPort**: Port for the REST API server (default: 8080)
- **bAutoPlayOnStart**: Whether to automatically play the default video on startup

## Usage

### In Unreal Editor

1. Open the project in Unreal Editor
2. Create a map or use the default map
3. The VR Movie Player Game Mode will automatically initialize all components
4. Press Play to test in VR Preview mode

### On Oculus Quest

1. Package the project for Android
2. Install the APK on your Oculus Quest device
3. Launch the application
4. Use the REST API to control playback from a remote device on the same network

### Example API Usage

```bash
# Get current video filename
curl http://<quest-ip>:8080/api/filename

# Get player state
curl http://<quest-ip>:8080/api/state

# Get current timestamp
curl http://<quest-ip>:8080/api/timestamp

# Get video duration
curl http://<quest-ip>:8080/api/duration
```

## Key Classes

### VideoPlayerController
Manages video playback using Unreal's Media Framework. Handles play, pause, stop, and seeking operations.

**Location**: `Source/VRMoviePlayer/VideoPlayerController.h/.cpp`

### VideoAPIServer
HTTP server that exposes REST API endpoints for remote control and querying of the video player.

**Location**: `Source/VRMoviePlayer/VideoAPIServer.h/.cpp`

### VRMovieScreen
Actor that creates and manages the virtual screen in VR space. Handles aspect ratio and positioning.

**Location**: `Source/VRMoviePlayer/VRMovieScreen.h/.cpp`

### VRMoviePlayerGameMode
Orchestrates all components, spawning and connecting the video player, API server, and screen.

**Location**: `Source/VRMoviePlayer/VRMoviePlayerGameMode.h/.cpp`

### VRPlayerPawn
VR player pawn with camera and motion controller support for Oculus Quest.

**Location**: `Source/VRMoviePlayer/VRPlayerPawn.h/.cpp`

## Performance Considerations

- The application is optimized for mobile VR (Quest) with 60+ FPS target
- Video playback uses hardware decoding when available
- Screen rendering uses unlit materials for optimal performance
- API responses are lightweight JSON with minimal processing

## Troubleshooting

### Video Not Playing
- Verify the video file path is correct and accessible
- Check that the video format is supported by Unreal's Media Framework
- Look for errors in the Output Log

### API Not Accessible
- Ensure the Quest device and client are on the same network
- Check firewall settings on both devices
- Verify the API port is not blocked
- Check the Output Log for server startup messages

### VR Not Working
- Ensure Oculus VR plugin is enabled
- Check that the Quest device is properly connected
- Verify VR is enabled in Project Settings
- Test with Oculus Link or Air Link for desktop development

## Future Enhancements

- Add POST endpoints for controlling playback (play, pause, stop, seek)
- Implement video playlist functionality
- Add subtitle support
- Implement UI controls within VR
- Add hand tracking support
- Support for streaming video sources

## License

Copyright Epic Games, Inc. All Rights Reserved.

## See Also

- [vision.md](vision.md) - Project vision and goals
- [requirements.md](requirements.md) - Detailed requirements specification
