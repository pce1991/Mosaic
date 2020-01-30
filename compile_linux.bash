
rm -rf build/
mkdir build/
gcc -O0 -Isrc -Ilib src/linux.cpp -o game -lx11 -lGL -lGLU -lGLEW 
