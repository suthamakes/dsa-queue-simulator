
include_directories(include)

add_library(client src/client.c)
add_library(renderer src/renderer.c)

find_package(SDL2 REQUIRED)
target_link_libraries(renderer SDL2)

add_executable(Simulator src/simulator.c)

target_link_libraries(Simulator client renderer)