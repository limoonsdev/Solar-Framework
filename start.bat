@echo off
setlocal EnableDelayedExpansion
chcp 65001 >nul
title Solar Framework
mode con: cols=88 lines=28
color 0D

:: ─────────────────────────────────────────────────────────────
:: Solar Framework — Premium Loader
:: ─────────────────────────────────────────────────────────────

set "ESC="
set "P=%ESC%[95m"
set "LP=%ESC%[35m"
set "W=%ESC%[97m"
set "G=%ESC%[90m"
set "D=%ESC%[2m"
set "R=%ESC%[0m"

cls

echo.
echo %D%    ┌──────────────────────────────────────────────────────────────────────────────┐%R%
echo %D%    │%R% %P%SOLAR%R% %G%/%R% %W%FRAMEWORK%R%                                                  %D%v1.1.0%D% %R%
echo %D%    ├──────────────────────────────────────────────────────────────────────────────┤%R%
echo %D%    │%R%
echo %D%    │%R%      %LP%╭────────────────────────────────────────────────────╮%R%
echo %D%    │%R%      %LP%│%R%                                                    %LP%│%R%
echo %D%    │%R%      %LP%│%R%              %P%S O L A R%R%                       %LP%│%R%
echo %D%    │%R%      %LP%│%R%          %G%PRIVATE FRAMEWORK%R%                  %LP%│%R%
echo %D%    │%R%      %LP%╰────────────────────────────────────────────────────╯%R%
echo %D%    │%R%
echo %D%    │%R%      %G%Status%R%       %P%●%R% %W%Ready%R%
echo %D%    │%R%      %G%Build%R%        %W%1.0.2-dev%R%
echo %D%    │%R%      %G%Developer%R%     %W%Limoons%R%
echo %D%    │%R%
echo %D%    ├──────────────────────────────────────────────────────────────────────────────┤%R%
echo %D%    │%R%
echo %D%    │%R%      %G%Discord%R%      %P%discord.gg/solarud%R%
echo %D%    │%R%      %G%Contact%R%      %W%@eqq7%R%
echo %D%    │%R%
echo %D%    │%R%      %P%[!]%R% %W%IF YOU BOUGHT THIS YOU HAVE BEEN SCAMMED...%R%
echo %D%    │%R%
echo %D%    └──────────────────────────────────────────────────────────────────────────────┘%R%
echo.

echo %G%    Initializing framework%R% %P%...%R%
timeout /t 1 /nobreak >nul

echo %G%    Loading modules%R%     %P%[████████████████████]%R%
timeout /t 1 /nobreak >nul

echo %G%    Verifying build%R%     %P%[████████████████████]%R%
timeout /t 1 /nobreak >nul

echo %G%    Framework status%R%   %P%●%R% %W%ONLINE%R%
echo.

echo %P%    ╭─[ SOLAR ]──────────────────────────────────────────────────────────────╮%R%
echo %P%    │%R% %W%Launching Solar Framework...%R%
echo %P%    ╰────────────────────────────────────────────────────────────────────────╯%R%
echo.

timeout /t 1 /nobreak >nul

"%~dp0build\Release\solar_demo.exe"

echo.
echo %G%    Solar Framework has been closed.%R%
echo.
pause