@echo off
set DEVKITPRO=C:\devkitPro
set DEVKITARM=%DEVKITPRO%\devkitARM
set DEVKITPPC=%DEVKITPRO%\devkitPPC
set PATH=%DEVKITPRO%\devkitARM\bin;%DEVKITPRO%\devkitPPC\bin;%DEVKITPRO%\tools\bin;%PATH%

if "%1"=="" (
    echo Building Multi-Platform Snake Game for GBA (default)
    make PLATFORM=gba
) else if "%1"=="gba" (
    echo Building Multi-Platform Snake Game for GBA
    make PLATFORM=gba
) else if "%1"=="nds" (
    echo Building Multi-Platform Snake Game for NDS
    make PLATFORM=nds
) else if "%1"=="ngc" (
    echo Building Multi-Platform Snake Game for GameCube
    make PLATFORM=ngc
) else if "%1"=="all" (
    echo Building Multi-Platform Snake Game for all platforms
    make all-platforms
) else (
    echo Usage: build.bat [gba^|nds^|ngc^|all]
    echo Default: gba
)
