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
include CMakeFiles/lab4_character.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/lab4_character.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/lab4_character.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/lab4_character.dir/flags.make

CMakeFiles/lab4_character.dir/lab4/lab4_character.cpp.o: CMakeFiles/lab4_character.dir/flags.make
CMakeFiles/lab4_character.dir/lab4/lab4_character.cpp.o: /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/lab4/lab4_character.cpp
CMakeFiles/lab4_character.dir/lab4/lab4_character.cpp.o: CMakeFiles/lab4_character.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/lab4_character.dir/lab4/lab4_character.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/lab4_character.dir/lab4/lab4_character.cpp.o -MF CMakeFiles/lab4_character.dir/lab4/lab4_character.cpp.o.d -o CMakeFiles/lab4_character.dir/lab4/lab4_character.cpp.o -c /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/lab4/lab4_character.cpp

CMakeFiles/lab4_character.dir/lab4/lab4_character.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/lab4_character.dir/lab4/lab4_character.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/lab4/lab4_character.cpp > CMakeFiles/lab4_character.dir/lab4/lab4_character.cpp.i

CMakeFiles/lab4_character.dir/lab4/lab4_character.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/lab4_character.dir/lab4/lab4_character.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/lab4/lab4_character.cpp -o CMakeFiles/lab4_character.dir/lab4/lab4_character.cpp.s

CMakeFiles/lab4_character.dir/lab4/render/shader.cpp.o: CMakeFiles/lab4_character.dir/flags.make
CMakeFiles/lab4_character.dir/lab4/render/shader.cpp.o: /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/lab4/render/shader.cpp
CMakeFiles/lab4_character.dir/lab4/render/shader.cpp.o: CMakeFiles/lab4_character.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/lab4_character.dir/lab4/render/shader.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/lab4_character.dir/lab4/render/shader.cpp.o -MF CMakeFiles/lab4_character.dir/lab4/render/shader.cpp.o.d -o CMakeFiles/lab4_character.dir/lab4/render/shader.cpp.o -c /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/lab4/render/shader.cpp

CMakeFiles/lab4_character.dir/lab4/render/shader.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/lab4_character.dir/lab4/render/shader.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/lab4/render/shader.cpp > CMakeFiles/lab4_character.dir/lab4/render/shader.cpp.i

CMakeFiles/lab4_character.dir/lab4/render/shader.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/lab4_character.dir/lab4/render/shader.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/lab4/render/shader.cpp -o CMakeFiles/lab4_character.dir/lab4/render/shader.cpp.s

# Object files for target lab4_character
lab4_character_OBJECTS = \
"CMakeFiles/lab4_character.dir/lab4/lab4_character.cpp.o" \
"CMakeFiles/lab4_character.dir/lab4/render/shader.cpp.o"

# External object files for target lab4_character
lab4_character_EXTERNAL_OBJECTS =

lab4_character: CMakeFiles/lab4_character.dir/lab4/lab4_character.cpp.o
lab4_character: CMakeFiles/lab4_character.dir/lab4/render/shader.cpp.o
lab4_character: CMakeFiles/lab4_character.dir/build.make
lab4_character: /usr/lib/x86_64-linux-gnu/libGL.so
lab4_character: /usr/lib/x86_64-linux-gnu/libGLU.so
lab4_character: libglfw.so.3.1
lab4_character: libglad.so
lab4_character: /usr/lib/x86_64-linux-gnu/librt.a
lab4_character: /usr/lib/x86_64-linux-gnu/libm.so
lab4_character: /usr/lib/x86_64-linux-gnu/libX11.so
lab4_character: /usr/lib/x86_64-linux-gnu/libXrandr.so
lab4_character: /usr/lib/x86_64-linux-gnu/libXinerama.so
lab4_character: /usr/lib/x86_64-linux-gnu/libXi.so
lab4_character: /usr/lib/x86_64-linux-gnu/libXxf86vm.so
lab4_character: /usr/lib/x86_64-linux-gnu/libXcursor.so
lab4_character: /usr/lib/x86_64-linux-gnu/libGL.so
lab4_character: CMakeFiles/lab4_character.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable lab4_character"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lab4_character.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/lab4_character.dir/build: lab4_character
.PHONY : CMakeFiles/lab4_character.dir/build

CMakeFiles/lab4_character.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/lab4_character.dir/cmake_clean.cmake
.PHONY : CMakeFiles/lab4_character.dir/clean

CMakeFiles/lab4_character.dir/depend:
	cd /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/build /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/build /home/roy/Documents/mylinuxfiles/Documents/college/Fourth/Graphics/project/build/CMakeFiles/lab4_character.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/lab4_character.dir/depend

