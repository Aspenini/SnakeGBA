@echo off
set DEVKITPRO=C:\devkitPro
set DEVKITARM=%DEVKITPRO%\devkitARM
set DEVKITPPC=%DEVKITPRO%\devkitPPC
set PATH=%DEVKITPRO%\devkitARM\bin;%DEVKITPRO%\devkitPPC\bin;%DEVKITPRO%\tools\bin;%PATH%

if "%1"=="" (
    echo Building Multi-Platform Snake Game for GBA (default)
    make PLATFORM=gba
    if %ERRORLEVEL% EQU 0 (
        echo.
        echo ✅ GBA build successful! snake.gba created.
        echo 🎮 Game features:
        echo    - Red apples that spawn 100%% reliably
        echo    - Score tracking and high score
        echo    - Optimized EWRAM performance
        echo    - Multi-platform architecture ready
    ) else (
        echo ❌ GBA build failed!
    )
) else if "%1"=="gba" (
    echo Building Multi-Platform Snake Game for GBA
    make PLATFORM=gba
    if %ERRORLEVEL% EQU 0 (
        echo ✅ GBA build successful!
    ) else (
        echo ❌ GBA build failed!
    )
) else if "%1"=="nds" (
    echo Building Multi-Platform Snake Game for NDS
    echo ⚠️  NDS build not fully implemented yet
    make PLATFORM=nds
) else if "%1"=="ngc" (
    echo Building Multi-Platform Snake Game for GameCube  
    echo ⚠️  GameCube build not fully implemented yet
    make PLATFORM=ngc
) else if "%1"=="all" (
    echo Building Multi-Platform Snake Game for all platforms
    echo ⚠️  Only GBA is fully implemented
    make PLATFORM=gba
) else (
    echo Usage: build.bat [gba^|nds^|ngc^|all]
    echo.
    echo Available platforms:
    echo   gba   - Game Boy Advance (fully working) ✅
    echo   nds   - Nintendo DS (not implemented) ⚠️
    echo   ngc   - GameCube (not implemented) ⚠️
    echo   all   - Build GBA only for now
    echo.
    echo Default: gba
)
