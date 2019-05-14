@ECHO OFF

IF NOT EXIST "%~dp0ToolLaunch.bat" (
  echo "%~dp0ToolLaunch.bat" does not exist
  pause
  exit /b
)

call "%~dp0ToolLaunch.bat" ruby.exe %~n0 %*
