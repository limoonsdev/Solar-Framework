@echo off
setlocal enabledelayedexpansion

echo ==============================================================================
echo [SOLAR STUDIO BETA v0.1.2] - Professional High-Performance Webview Launcher
echo ==============================================================================

set "STUDIO_ROOT=%~dp0"
set "HTML_TARGET=%STUDIO_ROOT%Solar Studio [BETA v0.1.2]\web\index.html"

if not exist "!HTML_TARGET!" (
    echo Error: Could not find web studio at !HTML_TARGET!
    pause
    exit /b 1
)

:: Try Microsoft Edge standalone application mode (Chromium GPU accelerated)
if exist "%ProgramFiles(x86)%\Microsoft\Edge\Application\msedge.exe" (
    echo Launching Solar Studio in Microsoft Edge App Mode...
    start "" "%ProgramFiles(x86)%\Microsoft\Edge\Application\msedge.exe" --app="file:///!HTML_TARGET:\=/!" --window-size=1440,920
    exit /b 0
)

if exist "%ProgramFiles%\Microsoft\Edge\Application\msedge.exe" (
    echo Launching Solar Studio in Microsoft Edge App Mode...
    start "" "%ProgramFiles%\Microsoft\Edge\Application\msedge.exe" --app="file:///!HTML_TARGET:\=/!" --window-size=1440,920
    exit /b 0
)

:: Try Google Chrome application mode
if exist "%ProgramFiles%\Google\Chrome\Application\chrome.exe" (
    echo Launching Solar Studio in Google Chrome App Mode...
    start "" "%ProgramFiles%\Google\Chrome\Application\chrome.exe" --app="file:///!HTML_TARGET:\=/!" --window-size=1440,920
    exit /b 0
)

:: Default browser fallback
echo Launching Solar Studio in default browser...
start "" "!HTML_TARGET!"
exit /b 0
