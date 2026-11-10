@echo off
rem ============================================================
rem  Launcher for setup-vscode-cp-env.ps1
rem  This file is intentionally ASCII-only: cmd.exe parses batch
rem  files with the *active* code page, so any UTF-8 Chinese here
rem  would be split mid-character and executed as commands.
rem  Use chcp 65001 + ASCII text, let PowerShell print the Chinese.
rem ============================================================
chcp 65001 >nul 2>&1
cd /d "%~dp0"
title L JUN - AMBITION STUDIO - CP Setup

echo.
echo   ============================================================
echo      L JUN  *  AMBITION STUDIO
echo      C/C++ Practice Environment Setup
echo   ============================================================
echo.

if not exist "%~dp0setup-vscode-cp-env.ps1" (
    echo   [FAIL] setup-vscode-cp-env.ps1 not found in this folder.
    echo          Keep this file, the .ps1 and winlibs-*.zip together.
    echo.
    pause
    exit /b 2
)

rem ---- preflight: check UTF-8 BOM, then clear the "downloaded" block ----
powershell -NoProfile -ExecutionPolicy Bypass -Command "$f='%~dp0setup-vscode-cp-env.ps1';$b=[IO.File]::ReadAllBytes($f);if($b.Length -lt 3 -or $b[0] -ne 239 -or $b[1] -ne 187 -or $b[2] -ne 191){Write-Host '';Write-Host '  [FAIL] 脚本缺少 UTF-8 BOM，几乎可以肯定在传输过程中被转码了。' -ForegroundColor Red;Write-Host '         中文会变乱码，甚至直接语法错误退出。' -ForegroundColor Yellow;Write-Host '         请从原始压缩包重新解压，不要用聊天窗口直接传 .ps1。' -ForegroundColor Yellow;Write-Host '';exit 2};Get-ChildItem '%~dp0*.ps1' -ErrorAction SilentlyContinue | Unblock-File -ErrorAction SilentlyContinue;exit 0"

if errorlevel 2 (
    pause
    exit /b 2
)

rem ---- run main script: bypass execution policy, pass through args ----
rem  CP_NO_PAUSE tells the script that this window will pause anyway
set "CP_NO_PAUSE=1"
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0setup-vscode-cp-env.ps1" %*
set "RC=%ERRORLEVEL%"

echo.
if not "%RC%"=="0" (
    echo   [exit code %RC%] See the red lines above.
) else (
    echo   Done.
)
echo.
pause
exit /b %RC%
