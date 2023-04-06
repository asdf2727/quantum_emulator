cmake_minimum_required(VERSION 3.23)
project(quantum_emulator)

set(CMAKE_CXX_STANDARD 14)

add_executable(quantum_emulator main.cpp state.cpp state.h transform.cpp transform.h all.h circuit.cpp circuit.h)
