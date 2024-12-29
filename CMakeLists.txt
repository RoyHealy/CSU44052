cmake_minimum_required(VERSION 3.0)
project(project)

find_package(OpenGL REQUIRED)
set (CMAKE_CXX_STANDARD 11)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

add_subdirectory(external)

include_directories(
	external/glfw-3.1.2/include/
	external/glm-0.9.7.1/
	external/glad-opengl-3.3/include/
	external/tinygltf-2.9.3/
	external/
	lab4/
	lab2/
)

add_executable(lab4_skeleton
	lab4/lab4_skeleton.cpp
	lab4/render/shader.cpp
)
target_link_libraries(lab4_skeleton
	${OPENGL_LIBRARY}
	glfw
	glad
)

add_executable(lab4_character
	lab4/lab4_character.cpp
	lab4/render/shader.cpp
)
target_link_libraries(lab4_character
	${OPENGL_LIBRARY}
	glfw
	glad
)
add_executable(buildings
	final-proj/buildings.cpp
	lab4/render/shader.cpp
)
target_link_libraries(buildings
	${OPENGL_LIBRARY}
	glfw
	glad
)

add_executable(lab2_skybox
	lab2/lab2_skybox.cpp
	lab2/render/shader.cpp
)
target_link_libraries(lab2_skybox
	${OPENGL_LIBRARY}
	glfw
	glad
)

add_executable(terrain
	final-proj/terrain.cpp
	lab4/render/shader.cpp
)
target_link_libraries(terrain
	${OPENGL_LIBRARY}
	glfw
	glad
)