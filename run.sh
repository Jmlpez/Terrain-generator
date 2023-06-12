#!/bin/bash
g++ imgui/*.cpp src/*.cpp main.cpp -o app glad.o -lglfw -lassimp -ldl && ./app
exit 1
