#!/bin/bash

pushd "$(dirname "${BASH_SOURCE[0]}")"
pushd ..

clang-format -i --verbose Source/OctaneGUI/*.cpp \
Source/OctaneGUI/*.h \
Source/OctaneGUI/Controls/*.h \
Source/OctaneGUI/Controls/*.cpp \
Source/OctaneGUI/Controls/Syntax/*.h \
Source/OctaneGUI/Controls/Syntax/*.cpp \
Source/OctaneGUI/Tools/*.h \
Source/OctaneGUI/Tools/*.cpp \
Frontends/*.h \
Frontends/*.cpp \
Frontends/Rendering/*.h \
Frontends/Rendering/Metal/*.* \
Frontends/Rendering/OpenGL/*.* \
Frontends/Rendering/SFML/*.* \
Frontends/Windowing/*.h \
Frontends/Windowing/SDL2/*.* \
Frontends/Windowing/SFML/*.*

popd
popd
