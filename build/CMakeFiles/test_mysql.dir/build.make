# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Produce verbose output by default.
VERBOSE = 1

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
CMAKE_COMMAND = /apps/sylar/bin/cmake

# The command to remove a file.
RM = /apps/sylar/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /root/workspace/sylar

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/workspace/sylar/build

# Include any dependencies generated for this target.
include CMakeFiles/test_mysql.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test_mysql.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test_mysql.dir/flags.make

CMakeFiles/test_mysql.dir/tests/test_mysql.cc.o: CMakeFiles/test_mysql.dir/flags.make
CMakeFiles/test_mysql.dir/tests/test_mysql.cc.o: ../tests/test_mysql.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/workspace/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/test_mysql.dir/tests/test_mysql.cc.o"
	/apps/sylar/bin/g++  $(CXX_DEFINES) -D__FILE__=\"tests/test_mysql.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_mysql.dir/tests/test_mysql.cc.o -c /root/workspace/sylar/tests/test_mysql.cc

CMakeFiles/test_mysql.dir/tests/test_mysql.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_mysql.dir/tests/test_mysql.cc.i"
	/apps/sylar/bin/g++ $(CXX_DEFINES) -D__FILE__=\"tests/test_mysql.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/workspace/sylar/tests/test_mysql.cc > CMakeFiles/test_mysql.dir/tests/test_mysql.cc.i

CMakeFiles/test_mysql.dir/tests/test_mysql.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_mysql.dir/tests/test_mysql.cc.s"
	/apps/sylar/bin/g++ $(CXX_DEFINES) -D__FILE__=\"tests/test_mysql.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/workspace/sylar/tests/test_mysql.cc -o CMakeFiles/test_mysql.dir/tests/test_mysql.cc.s

# Object files for target test_mysql
test_mysql_OBJECTS = \
"CMakeFiles/test_mysql.dir/tests/test_mysql.cc.o"

# External object files for target test_mysql
test_mysql_EXTERNAL_OBJECTS =

../bin/test_mysql: CMakeFiles/test_mysql.dir/tests/test_mysql.cc.o
../bin/test_mysql: CMakeFiles/test_mysql.dir/build.make
../bin/test_mysql: ../lib/libsylar.so
../bin/test_mysql: /usr/lib64/libz.so
../bin/test_mysql: /usr/lib64/libssl.so
../bin/test_mysql: /usr/lib64/libcrypto.so
../bin/test_mysql: /apps/sylar/lib/libprotobuf.so
../bin/test_mysql: CMakeFiles/test_mysql.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/root/workspace/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/test_mysql"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_mysql.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test_mysql.dir/build: ../bin/test_mysql

.PHONY : CMakeFiles/test_mysql.dir/build

CMakeFiles/test_mysql.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_mysql.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_mysql.dir/clean

CMakeFiles/test_mysql.dir/depend:
	cd /root/workspace/sylar/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/workspace/sylar /root/workspace/sylar /root/workspace/sylar/build /root/workspace/sylar/build /root/workspace/sylar/build/CMakeFiles/test_mysql.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test_mysql.dir/depend

