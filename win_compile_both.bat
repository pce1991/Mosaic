
@echo OFF

Rem maybe you dont want to do this so you can keep the server running and just recompile client.
RMDIR /S /Q build\
MKDIR build\
cl /O0 /Zi /I lib /D IS_SERVER=1 src\windows.cpp /Febuild\server.exe user32.lib gdi32.lib ole32.lib lib\glew32s.lib opengl32.lib  /link ws2_32.lib /incremental:no

cl /O0 /Zi /I lib /D IS_SERVER=0 src\windows.cpp /Febuild\game.exe user32.lib gdi32.lib ole32.lib lib\glew32s.lib opengl32.lib  /link ws2_32.lib /incremental:no
