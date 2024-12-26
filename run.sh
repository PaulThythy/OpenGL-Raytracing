#!/bin/bash

cmake -S . -B build/ -G "Ninja"
cmake --build build/ --target OpenglRaytracer

#Check if the compilation was successful
if [ $? -eq 0 ]; then
	echo "Compilation successful. Running the Raytracer..."
	#Execute the raytracer binary
	./build/OpenglRaytracer
else
    echo "Compilation failed. Please check the error messages."
fi