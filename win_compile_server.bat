
@echo OFF

RMDIR /S /Q build\
MKDIR build\
cl /O0 /Zi /I lib /D IS_SERVER=1 src\windows.cpp /Febuild\server.exe user32.lib gdi32.lib ole32.lib lib\glew32s.lib opengl32.lib  /link ws2_32.lib /incremental:no


if "%1" == "run" start build\server.exe exit
