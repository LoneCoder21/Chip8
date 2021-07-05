cmake_minimum_required(VERSION 3.10)
project(Chip8)
set(CMAKE_CXX_STANDARD 17)

link_directories(vendor/SDL2/lib/x64)

file(GLOB Cs "*.h" "*.cpp")
add_executable(Chip8 ${Cs})

target_include_directories(Chip8 PRIVATE vendor/SDL2/include)
target_link_libraries(Chip8 SDL2.lib)
target_link_libraries(Chip8 SDL2main.lib)

file(COPY vendor/SDL2/lib/x64/SDL2.dll DESTINATION Debug/)
file(COPY vendor/SDL2/lib/x64/SDL2.dll DESTINATION Release/)
