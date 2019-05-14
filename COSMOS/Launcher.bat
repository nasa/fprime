@ECHO OFF

IF NOT EXIST "%~dp0tools\ToolLaunch.bat" (
  echo "%~dp0tools\ToolLaunch.bat" does not exist
  pause
  exit /b
)

call "%~dp0tools\ToolLaunch.bat" rubyw.exe %~n0 %*
