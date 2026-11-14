param(
    [string]$EngineRoot = 'I:\EpicLauncher\UE_4.27',
    [string]$AndroidSdk = "$env:LOCALAPPDATA\Android\Sdk-UE427",
    [string]$AndroidNdk = "$env:LOCALAPPDATA\Android\Sdk\ndk\21.4.7075529",
    [string]$JavaHome = 'I:\Program Files\Android\Android Studio1\jre'
)

$editor = Join-Path $EngineRoot 'Engine\Binaries\Win64\UE4Editor.exe'
foreach ($path in @($editor, $AndroidSdk, $AndroidNdk, (Join-Path $JavaHome 'bin\java.exe'))) {
    if (-not (Test-Path -LiteralPath $path)) {
        throw "Required UE 4.27 toolchain path does not exist: $path"
    }
}

$env:ANDROID_HOME = $AndroidSdk
$env:NDKROOT = $AndroidNdk
$env:JAVA_HOME = $JavaHome
$env:ANDROID_SDK_HOME = "$env:LOCALAPPDATA\Android\UE427User"
New-Item -ItemType Directory -Force -Path $env:ANDROID_SDK_HOME | Out-Null

& $editor (Join-Path $PSScriptRoot 'VRMoviePlayer.uproject')
exit $LASTEXITCODE
