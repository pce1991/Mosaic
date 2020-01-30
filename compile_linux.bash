
rm -rf build/
mkdir build/
gcc -O0 src/linux.cpp -o game -lx11 -lGL -lGLU -lGLEW
