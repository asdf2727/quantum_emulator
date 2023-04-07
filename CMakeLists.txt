cmake_minimum_required(VERSION 3.23)
project(quantum_emulator)
project(quantum_error_correction)

set(CMAKE_CXX_STANDARD 14)

add_executable(quantum_emulator teleport.cpp state.cpp state.h transform.cpp transform.h all.h circuit.cpp circuit.h)
add_executable(quantum_error_correction error.cpp state.cpp state.h transform.cpp transform.h all.h circuit.cpp circuit.h)
