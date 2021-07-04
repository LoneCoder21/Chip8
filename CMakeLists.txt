cmake_minimum_required(VERSION 3.10)

add_subdirectory("vendor/SFML")

project(LevelEditor)

set(CMAKE_CXX_STANDARD 17)

link_directories("vendor/SFML/lib")

add_executable(LevelEditor Main.cpp)

target_link_libraries(LevelEditor debug sfml-graphics-d.lib optimized sfml-graphics.lib)
target_link_libraries(LevelEditor debug sfml-window-d.lib optimized sfml-window.lib)
target_link_libraries(LevelEditor debug sfml-system-d.lib optimized sfml-system.lib)

target_include_directories(LevelEditor PRIVATE "vendor/SFML/include")

file(COPY vendor/SFML/lib/Debug/sfml-graphics-d-2.dll DESTINATION Debug/)
file(COPY vendor/SFML/lib/Debug/sfml-window-d-2.dll DESTINATION Debug/)
file(COPY vendor/SFML/lib/Debug/sfml-system-d-2.dll DESTINATION Debug/)

file(COPY vendor/SFML/lib/Release/sfml-graphics-2.dll DESTINATION Release)
file(COPY vendor/SFML/lib/Release/sfml-window-2.dll DESTINATION Release)
file(COPY vendor/SFML/lib/Release/sfml-system-2.dll DESTINATION Release)