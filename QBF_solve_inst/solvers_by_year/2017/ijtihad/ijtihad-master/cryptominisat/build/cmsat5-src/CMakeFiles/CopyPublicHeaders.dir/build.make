# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.7

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
CMAKE_SOURCE_DIR = /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build

# Utility rule file for CopyPublicHeaders.

# Include the progress variables for this target.
include cmsat5-src/CMakeFiles/CopyPublicHeaders.dir/progress.make

CopyPublicHeaders: cmsat5-src/CMakeFiles/CopyPublicHeaders.dir/build.make
	cd /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/cmsat5-src && /usr/bin/cmake -E make_directory /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/include/cryptominisat5
	cd /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/cmsat5-src && /usr/bin/cmake -E echo Copying\ cryptominisat_c.h\ to\ /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/include/cryptominisat5
	cd /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/cmsat5-src && /usr/bin/cmake -E copy_if_different /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/cmsat5-src/cryptominisat5/cryptominisat_c.h /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/include/cryptominisat5
	cd /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/cmsat5-src && /usr/bin/cmake -E make_directory /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/include/cryptominisat5
	cd /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/cmsat5-src && /usr/bin/cmake -E echo Copying\ cryptominisat.h\ to\ /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/include/cryptominisat5
	cd /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/cmsat5-src && /usr/bin/cmake -E copy_if_different /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/cmsat5-src/cryptominisat5/cryptominisat.h /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/include/cryptominisat5
	cd /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/cmsat5-src && /usr/bin/cmake -E make_directory /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/include/cryptominisat5
	cd /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/cmsat5-src && /usr/bin/cmake -E echo Copying\ solvertypesmini.h\ to\ /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/include/cryptominisat5
	cd /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/cmsat5-src && /usr/bin/cmake -E copy_if_different /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/cmsat5-src/cryptominisat5/solvertypesmini.h /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/include/cryptominisat5
.PHONY : CopyPublicHeaders

# Rule to build all files generated by this target.
cmsat5-src/CMakeFiles/CopyPublicHeaders.dir/build: CopyPublicHeaders

.PHONY : cmsat5-src/CMakeFiles/CopyPublicHeaders.dir/build

cmsat5-src/CMakeFiles/CopyPublicHeaders.dir/clean:
	cd /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/cmsat5-src && $(CMAKE_COMMAND) -P CMakeFiles/CopyPublicHeaders.dir/cmake_clean.cmake
.PHONY : cmsat5-src/CMakeFiles/CopyPublicHeaders.dir/clean

cmsat5-src/CMakeFiles/CopyPublicHeaders.dir/depend:
	cd /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/src /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/cmsat5-src /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/cmsat5-src/CMakeFiles/CopyPublicHeaders.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : cmsat5-src/CMakeFiles/CopyPublicHeaders.dir/depend

