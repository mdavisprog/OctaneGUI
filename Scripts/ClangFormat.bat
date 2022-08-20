@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION

PUSHD "%~dp0"

clang-format -i --verbose^
 ..\Source/OctaneGUI/*.cpp^
 ..\Source/OctaneGUI/*.h^
 ..\Source/OctaneGUI/Controls/*.cpp^
 ..\Source/OctaneGUI/Controls/*.h^
 ..\Source/OctaneGUI/Controls/Syntax/*.cpp^
 ..\Source/OctaneGUI/Controls/Syntax/*.h^
 ..\Source/OctaneGUI/Dialogs/*.cpp^
 ..\Source/OctaneGUI/Dialogs/*.h^
 ..\Source/OctaneGUI/Tools/*.h^
 ..\Source/OctaneGUI/Tools/*.cpp^
 ..\Frontends/*.h^
 ..\Frontends/*.cpp^
 ..\Frontends/Rendering/*.h^
 ..\Frontends/Rendering/OpenGL/*.cpp^
 ..\Frontends/Rendering/SFML/*.cpp^
 ..\Frontends/Windowing/*.h^
 ..\Frontends/Windowing/SDL2/*.h^
 ..\Frontends/Windowing/SDL2/*.cpp^
 ..\Frontends/Windowing/SFML/*.h^
 ..\Frontends/Windowing/SFML/*.cpp

POPD

ENDLOCAL