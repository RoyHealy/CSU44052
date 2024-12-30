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

add_executable(camera
	final-proj/camera.cpp
	lab4/render/shader.cpp
)
target_link_libraries(camera
	${OPENGL_LIBRARY}
	glfw
	glad
)

add_executable(tempppp
	final-proj/temp.cpp
	lab2/render/shader.cpp
)
target_link_libraries(tempppp
	${OPENGL_LIBRARY}
	glfw
	glad
)
