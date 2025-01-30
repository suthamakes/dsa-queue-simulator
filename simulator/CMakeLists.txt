
include_directories(include)

add_library(client src/client.c)

add_executable(Simulator src/simulator.c)
target_link_libraries(Simulator client)