
rm -rf build/
mkdir build/
gcc -g -O2 -Isrc -Ilib src/linux.cpp -o build/game -lX11 -lGL -lGLU -lGLEW -lm -lpthread -std=c++11 -w
