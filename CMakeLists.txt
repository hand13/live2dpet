project(Actor)
cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 17)
include_directories("e:/usr/inc" "Framework\\src"
                "E:\\usr\\local\\live2d\\core\\include")
link_directories("e:/usr/lib" "E:\\usr\\local\\live2d\\core\\lib\\windows\\x86_64\\142")

# Specify Cubism Framework rendering.
set(FRAMEWORK_SOURCE D3D11)
# Add Cubism Native Framework.
add_subdirectory(Framework ${CMAKE_CURRENT_BINARY_DIR}/Framework)
# Add include path to framework.
target_include_directories(Framework PUBLIC e:/usr/Inc)
# Link libraries to framework.
target_link_libraries(Framework
  Live2DCubismCore_MDd
  e:/usr/lib/DirectXTK.lib
  d3d11.lib
  d3dcompiler.lib
)

add_executable(game main.cpp game.cpp shader.cpp sprite.cpp util.cpp LAppAllocator.cpp LAppModel.cpp text_renderer.cpp)
target_link_libraries(game d3d11 d3dcompiler DirectXTK dcomp dxgi Live2DCubismCore_MDd Framework)
add_custom_command(TARGET game PRE_BUILD
 COMMAND ${CMAKE_COMMAND} -E copy_directory
 	${CMAKE_SOURCE_DIR}/resource $<TARGET_FILE_DIR:game>/resource)