cmake_minimum_required(VERSION 3.23)
project(quantum_emulator)
project(quantum_error_correction)

set(CMAKE_CXX_STANDARD 14)

add_executable(quantum_emulator teleport.cpp state.h transform.h circuit.h)
add_executable(quantum_error_correction error.cpp state.h transform.h circuit.h)
