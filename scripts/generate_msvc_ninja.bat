@echo off
mkdir ..\build\msvc_ninja
cd ..\build\msvc_ninja
cmake ../.. -G"Ninja Multi-Config" -DVCPKG_TARGET_TRIPLET=x64-windows-static-md -DCMAKE_TOOLCHAIN_FILE=c:/vcpkg/scripts/buildsystems/vcpkg.cmake
