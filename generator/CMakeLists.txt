
include_directories(include)

add_library(server src/server.c)

add_executable(Generator src/traffic_generator.c)
target_link_libraries(Generator server)