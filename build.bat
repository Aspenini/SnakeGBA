@echo off
set DEVKITPRO=C:\devkitPro
set DEVKITARM=%DEVKITPRO%\devkitARM
set PATH=%DEVKITPRO%\devkitARM\bin;%DEVKITPRO%\tools\bin;%PATH%
echo Building Enhanced Snake Game for GBA
make
