cmake_minimum_required(VERSION 3.10)

project(
    hoperload 
    VERSION 1.0
)

include("$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

add_executable(${PROJECT_NAME}     
    src/main.cpp
    src/Camera.cpp
    src/GUI.cpp
    src/Hoperload.cpp
    src/Maths.cpp
    src/Player.cpp
    src/Utility.cpp
    src/World.cpp

    src/Chunks/Chunk.cpp
    src/Chunks/ChunkMap.cpp
    src/Chunks/Conversions.cpp
    src/Chunks/ChunkTerrainGen.cpp
    src/Chunks/ChunkMesh.cpp
    src/Chunks/Voxels.cpp


    src/Graphics/Framebuffer.cpp
    src/Graphics/Mesh.cpp
    src/Graphics/Shader.cpp
    src/Graphics/Texture.cpp
    src/Graphics/VertexArray.cpp
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

target_compile_definitions(${PROJECT_NAME} PRIVATE GLM_ENABLE_EXPERIMENTAL)

if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "/O2")
    set(CMAKE_CXX_FLAGS_RELEASE "/Ox")
  	target_compile_options(${PROJECT_NAME} PRIVATE /W4 _CRT_SECURE_NO_WARNINGS)
else()
  	target_compile_options(${PROJECT_NAME} PRIVATE 
		-Wall -Wextra -pedantic)		
endif()

find_package(glm CONFIG REQUIRED)
find_package(SFML COMPONENTS system audio network window graphics CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

add_subdirectory(deps)
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    deps
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    sfml-system sfml-audio sfml-network sfml-graphics sfml-window
    imgui::imgui
    glm::glm
    imgui_sfml
    glad 
)
