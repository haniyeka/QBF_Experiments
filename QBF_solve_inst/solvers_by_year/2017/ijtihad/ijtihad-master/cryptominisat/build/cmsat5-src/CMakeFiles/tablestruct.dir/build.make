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

# Utility rule file for tablestruct.

# Include the progress variables for this target.
include cmsat5-src/CMakeFiles/tablestruct.dir/progress.make

cmsat5-src/CMakeFiles/tablestruct: cmsat5-src/sql_tablestructure.cpp


cmsat5-src/sql_tablestructure.cpp: ../cmsat_tablestructure.sql
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating sql_tablestructure.cpp"
	cd /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat && xxd -i cmsat_tablestructure.sql /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/cmsat5-src/sql_tablestructure.cpp

tablestruct: cmsat5-src/CMakeFiles/tablestruct
tablestruct: cmsat5-src/sql_tablestructure.cpp
tablestruct: cmsat5-src/CMakeFiles/tablestruct.dir/build.make

.PHONY : tablestruct

# Rule to build all files generated by this target.
cmsat5-src/CMakeFiles/tablestruct.dir/build: tablestruct

.PHONY : cmsat5-src/CMakeFiles/tablestruct.dir/build

cmsat5-src/CMakeFiles/tablestruct.dir/clean:
	cd /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/cmsat5-src && $(CMAKE_COMMAND) -P CMakeFiles/tablestruct.dir/cmake_clean.cmake
.PHONY : cmsat5-src/CMakeFiles/tablestruct.dir/clean

cmsat5-src/CMakeFiles/tablestruct.dir/depend:
	cd /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/src /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/cmsat5-src /home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/cmsat5-src/CMakeFiles/tablestruct.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : cmsat5-src/CMakeFiles/tablestruct.dir/depend

