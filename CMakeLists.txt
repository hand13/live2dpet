project(Actor)
add_executable(game main.cpp game.cpp)
target_link_libraries(game d3d11 d3dcompiler)