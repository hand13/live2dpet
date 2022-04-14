project(Actor)
cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 17)
include_directories("e:/usr/inc" 
                "E:\\usr\\local\\live2d\\framework\\inc"
                "E:\\usr\\local\\live2d\\core\\include")
link_directories("e:/usr/lib" "E:\\usr\\local\\live2d\\core\\lib\\windows\\x86_64\\142" "E:\\usr\\local\\live2d\\framework\\lib")
add_executable(game main.cpp game.cpp shader.cpp sprite.cpp util.cpp LAppAllocator.cpp LAppModel.cpp)
target_link_libraries(game d3d11 d3dcompiler DirectXTK dcomp dxgi Live2DCubismCore_MDd Framework)
add_custom_command(TARGET game PRE_BUILD
 COMMAND ${CMAKE_COMMAND} -E copy_directory
 	${CMAKE_SOURCE_DIR}/resource $<TARGET_FILE_DIR:game>/resource)