mkdir ..\build\mingw-w64
cd ..\build\mingw-w64
cmake ../.. -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -G"Ninja Multi-Config"
