@echo off
if not exist build\debug mkdir build\debug
if not exist build\release mkdir build\release

cd build\debug
cmake ..\.. -DCMAKE_BUILD_TYPE=Debug -G"Visual Studio 16 2019"
cd ..\release
cmake ..\.. -DCMAKE_BUILD_TYPE=Release -G"Visual Studio 16 2019"
cd ..\..\buildScripts