# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/gennart/Documents/MAI/Thesis/spn_sw

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/gennart/Documents/MAI/Thesis/spn_sw/build

# Include any dependencies generated for this target.
include src/CMakeFiles/SPN.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/SPN.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/SPN.dir/flags.make

src/CMakeFiles/SPN.dir/spn.cpp.o: src/CMakeFiles/SPN.dir/flags.make
src/CMakeFiles/SPN.dir/spn.cpp.o: ../src/spn.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gennart/Documents/MAI/Thesis/spn_sw/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/SPN.dir/spn.cpp.o"
	cd /home/gennart/Documents/MAI/Thesis/spn_sw/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/SPN.dir/spn.cpp.o -c /home/gennart/Documents/MAI/Thesis/spn_sw/src/spn.cpp

src/CMakeFiles/SPN.dir/spn.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SPN.dir/spn.cpp.i"
	cd /home/gennart/Documents/MAI/Thesis/spn_sw/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gennart/Documents/MAI/Thesis/spn_sw/src/spn.cpp > CMakeFiles/SPN.dir/spn.cpp.i

src/CMakeFiles/SPN.dir/spn.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SPN.dir/spn.cpp.s"
	cd /home/gennart/Documents/MAI/Thesis/spn_sw/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gennart/Documents/MAI/Thesis/spn_sw/src/spn.cpp -o CMakeFiles/SPN.dir/spn.cpp.s

# Object files for target SPN
SPN_OBJECTS = \
"CMakeFiles/SPN.dir/spn.cpp.o"

# External object files for target SPN
SPN_EXTERNAL_OBJECTS =

src/libSPN.a: src/CMakeFiles/SPN.dir/spn.cpp.o
src/libSPN.a: src/CMakeFiles/SPN.dir/build.make
src/libSPN.a: src/CMakeFiles/SPN.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/gennart/Documents/MAI/Thesis/spn_sw/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libSPN.a"
	cd /home/gennart/Documents/MAI/Thesis/spn_sw/build/src && $(CMAKE_COMMAND) -P CMakeFiles/SPN.dir/cmake_clean_target.cmake
	cd /home/gennart/Documents/MAI/Thesis/spn_sw/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/SPN.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/SPN.dir/build: src/libSPN.a

.PHONY : src/CMakeFiles/SPN.dir/build

src/CMakeFiles/SPN.dir/clean:
	cd /home/gennart/Documents/MAI/Thesis/spn_sw/build/src && $(CMAKE_COMMAND) -P CMakeFiles/SPN.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/SPN.dir/clean

src/CMakeFiles/SPN.dir/depend:
	cd /home/gennart/Documents/MAI/Thesis/spn_sw/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/gennart/Documents/MAI/Thesis/spn_sw /home/gennart/Documents/MAI/Thesis/spn_sw/src /home/gennart/Documents/MAI/Thesis/spn_sw/build /home/gennart/Documents/MAI/Thesis/spn_sw/build/src /home/gennart/Documents/MAI/Thesis/spn_sw/build/src/CMakeFiles/SPN.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/SPN.dir/depend
