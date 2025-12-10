# Requirements

## Functional Requirements

### FR1: Video Playback
- FR1.1: The application SHALL play video files in VR environment
- FR1.2: The application SHALL render video content on a virtual screen
- FR1.3: The application SHALL support standard video formats (MP4, AVI, MKV, etc.)
- FR1.4: The application SHALL support play, pause, and stop operations
- FR1.5: The application SHALL track and maintain current playback timestamp

### FR2: REST API Endpoints
- FR2.1: GET endpoint to retrieve current video filename
- FR2.2: GET endpoint to retrieve player state (playing/paused/stopped)
- FR2.3: GET endpoint to retrieve current video timestamp
- FR2.4: GET endpoint to retrieve total video length/duration
- FR2.5: All API endpoints SHALL return data in JSON format
- FR2.6: API SHALL be accessible over HTTP/HTTPS

### FR3: VR Display
- FR3.1: The application SHALL create a virtual screen in 3D space
- FR3.2: The screen SHALL be properly positioned for comfortable viewing
- FR3.3: The application SHALL maintain proper aspect ratio of video content
- FR3.4: The virtual environment SHALL provide adequate lighting for screen visibility

## Non-Functional Requirements

### NFR1: Performance
- NFR1.1: Video playback SHALL maintain minimum 60 FPS in VR
- NFR1.2: API response time SHALL be under 100ms
- NFR1.3: Frame drops SHALL be minimized to prevent VR discomfort

### NFR2: Platform
- NFR2.1: Application SHALL run on Oculus Quest platform
- NFR2.2: Application SHALL be built using Unreal Engine 5
- NFR2.3: Application SHALL comply with Oculus Quest performance guidelines

### NFR3: Usability
- NFR3.1: Video controls SHALL be accessible within VR environment
- NFR3.2: REST API SHALL have clear and consistent endpoint naming

### NFR4: Reliability
- NFR4.1: Application SHALL handle missing or corrupted video files gracefully
- NFR4.2: API SHALL return appropriate error codes for invalid requests
- NFR4.3: Application SHALL not crash when video playback encounters errors
