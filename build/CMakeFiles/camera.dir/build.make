# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/build

# Include any dependencies generated for this target.
include CMakeFiles/camera.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/camera.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/camera.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/camera.dir/flags.make

CMakeFiles/camera.dir/final-proj/camera.cpp.o: CMakeFiles/camera.dir/flags.make
CMakeFiles/camera.dir/final-proj/camera.cpp.o: /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/final-proj/camera.cpp
CMakeFiles/camera.dir/final-proj/camera.cpp.o: CMakeFiles/camera.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/camera.dir/final-proj/camera.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/camera.dir/final-proj/camera.cpp.o -MF CMakeFiles/camera.dir/final-proj/camera.cpp.o.d -o CMakeFiles/camera.dir/final-proj/camera.cpp.o -c /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/final-proj/camera.cpp

CMakeFiles/camera.dir/final-proj/camera.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/camera.dir/final-proj/camera.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/final-proj/camera.cpp > CMakeFiles/camera.dir/final-proj/camera.cpp.i

CMakeFiles/camera.dir/final-proj/camera.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/camera.dir/final-proj/camera.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/final-proj/camera.cpp -o CMakeFiles/camera.dir/final-proj/camera.cpp.s

CMakeFiles/camera.dir/lab4/render/shader.cpp.o: CMakeFiles/camera.dir/flags.make
CMakeFiles/camera.dir/lab4/render/shader.cpp.o: /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/lab4/render/shader.cpp
CMakeFiles/camera.dir/lab4/render/shader.cpp.o: CMakeFiles/camera.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/camera.dir/lab4/render/shader.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/camera.dir/lab4/render/shader.cpp.o -MF CMakeFiles/camera.dir/lab4/render/shader.cpp.o.d -o CMakeFiles/camera.dir/lab4/render/shader.cpp.o -c /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/lab4/render/shader.cpp

CMakeFiles/camera.dir/lab4/render/shader.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/camera.dir/lab4/render/shader.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/lab4/render/shader.cpp > CMakeFiles/camera.dir/lab4/render/shader.cpp.i

CMakeFiles/camera.dir/lab4/render/shader.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/camera.dir/lab4/render/shader.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/lab4/render/shader.cpp -o CMakeFiles/camera.dir/lab4/render/shader.cpp.s

# Object files for target camera
camera_OBJECTS = \
"CMakeFiles/camera.dir/final-proj/camera.cpp.o" \
"CMakeFiles/camera.dir/lab4/render/shader.cpp.o"

# External object files for target camera
camera_EXTERNAL_OBJECTS =

camera: CMakeFiles/camera.dir/final-proj/camera.cpp.o
camera: CMakeFiles/camera.dir/lab4/render/shader.cpp.o
camera: CMakeFiles/camera.dir/build.make
camera: /usr/lib/x86_64-linux-gnu/libGL.so
camera: /usr/lib/x86_64-linux-gnu/libGLU.so
camera: libglfw.so.3.1
camera: libglad.so
camera: /usr/lib/x86_64-linux-gnu/librt.a
camera: /usr/lib/x86_64-linux-gnu/libm.so
camera: /usr/lib/x86_64-linux-gnu/libX11.so
camera: /usr/lib/x86_64-linux-gnu/libXrandr.so
camera: /usr/lib/x86_64-linux-gnu/libXinerama.so
camera: /usr/lib/x86_64-linux-gnu/libXi.so
camera: /usr/lib/x86_64-linux-gnu/libXxf86vm.so
camera: /usr/lib/x86_64-linux-gnu/libXcursor.so
camera: /usr/lib/x86_64-linux-gnu/libGL.so
camera: CMakeFiles/camera.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable camera"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/camera.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/camera.dir/build: camera
.PHONY : CMakeFiles/camera.dir/build

CMakeFiles/camera.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/camera.dir/cmake_clean.cmake
.PHONY : CMakeFiles/camera.dir/clean

CMakeFiles/camera.dir/depend:
	cd /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/build /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/build /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/build/CMakeFiles/camera.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/camera.dir/depend
