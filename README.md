# VR Movie Player

A VR movie player application for Oculus Quest 1 built with Unreal Engine 4.27.2. This application allows you to watch videos on a virtual screen in VR and query playback through a REST API.

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

- Unreal Engine 4.27.2 with Android support
- Oculus Quest device
- Android development tools (for Quest deployment)
- Visual Studio 2019/2022 (Windows) or Xcode (Mac)

## Building the Project

Build the editor target (adjust paths for your installation):

```powershell
& "I:\EpicLauncher\UE_4.27\Engine\Build\BatchFiles\Build.bat" VRMoviePlayerEditor Win64 Development -Project="F:\devel\git\vrmovieplayer\VRMoviePlayer.uproject" -WaitMutex -NoHotReloadFromIDE
.\Launch-UE427.ps1
```

The launcher uses a separate Android debug keystore location for Java 8. Its parameters
can be changed for other installations. UE 4.27's Android SDK settings are shared with
other Unreal versions; switch them back before building with UE 5.

### Package for Oculus Quest

Use UE 4.27's bundled Oculus VR plugin, not the UE 5 OpenXR loader replacement.
Configure the Android SDK, NDK and JDK for UE 4.27 following
[Epic's Android setup guide](https://dev.epicgames.com/documentation/en-us/unreal-engine/setting-up-android-sdk-and-ndk-for-unreal?application_version=4.27).
The engine's `SetupAndroid.bat` requests NDK 21.4.7075529; newer NDKs used by UE 5 are not supported.

1. In Unreal Editor: **File → Package Project → Android → Android (ASTC)**
2. Choose output folder
3. Wait for packaging to complete
4. Install on Quest: `adb install -r output.apk`

Quest 1 is no longer supported for new Meta Store updates; use a development-enabled headset and sideload the APK.
The UE 5 assets were replaced by a minimal UE 4.27 level and video material; the original assets remain on the original Git branch.

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

### Analyzing Logs on Quest

The app writes a full UE log to device storage. This is the most useful source of crash and error information.

**Pull and read the UE log:**
```powershell
$platformTools = "C:\Users\TheJudge\AppData\Local\Android\Sdk\platform-tools"
$logPath = "/sdcard/Android/data/com.vrmovieplayer.app/files/UnrealGame/VRMoviePlayer/VRMoviePlayer/Saved/Logs/VRMoviePlayer.log"
& "$platformTools\adb.exe" pull $logPath "$env:TEMP\VRMoviePlayer.log"
Get-Content "$env:TEMP\VRMoviePlayer.log" | Select-Object -Last 100
```

**Capture live logcat while the app runs** (clear first, launch app, then dump):
```powershell
$platformTools = "C:\Users\TheJudge\AppData\Local\Android\Sdk\platform-tools"
& "$platformTools\adb.exe" logcat -c
# Launch the app on the headset, then wait ~20 seconds, then run:
& "$platformTools\adb.exe" logcat -d -v brief "UE:V" "*:E" | Select-Object -Last 200
```

**Search for errors and fatal crashes in the UE log:**
```powershell
Get-Content "$env:TEMP\VRMoviePlayer.log" | Select-String -Pattern "Error|Fatal|Critical" | Select-Object -Last 50
```

**Check if the app process is running:**
```powershell
$platformTools = "C:\Users\TheJudge\AppData\Local\Android\Sdk\platform-tools"
& "$platformTools\adb.exe" shell pidof com.vrmovieplayer.app
```

### Video Not Playing
- Verify the video file path is correct and accessible
- Check that the video format is supported by Unreal's Media Framework
- Look for errors in the UE log (see above)

### API Not Accessible
- Ensure the Quest device and client are on the same network
- Check firewall settings on both devices
- Verify the API port is not blocked
- Check the UE log for server startup messages

### VR Not Working
- Ensure the app is listed under **App Library → Unknown Sources** (not as a flat 2D app)
- Verify `com.oculus.intent.category.VR` is present in `ExtraActivitySettings` in `DefaultEngine.ini`
- Check that `quest` is listed in the `com.oculus.supportedDevices` metadata value
- Test desktop fallback by running in Unreal Editor (non-VR mode is supported)

### No REST API access
- Check access without WIFI: `adb reverse tcp:8080 tcp:8080`

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
