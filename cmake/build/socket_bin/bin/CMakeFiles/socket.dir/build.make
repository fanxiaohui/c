# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.1

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/anton/program/c/cmake

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/anton/program/c/cmake/build

# Include any dependencies generated for this target.
include socket_bin/bin/CMakeFiles/socket.dir/depend.make

# Include the progress variables for this target.
include socket_bin/bin/CMakeFiles/socket.dir/progress.make

# Include the compile flags for this target's objects.
include socket_bin/bin/CMakeFiles/socket.dir/flags.make

socket_bin/bin/CMakeFiles/socket.dir/main/socket.c.o: socket_bin/bin/CMakeFiles/socket.dir/flags.make
socket_bin/bin/CMakeFiles/socket.dir/main/socket.c.o: ../socket/src/main/socket.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/anton/program/c/cmake/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object socket_bin/bin/CMakeFiles/socket.dir/main/socket.c.o"
	cd /home/anton/program/c/cmake/build/socket_bin/bin && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/socket.dir/main/socket.c.o   -c /home/anton/program/c/cmake/socket/src/main/socket.c

socket_bin/bin/CMakeFiles/socket.dir/main/socket.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/socket.dir/main/socket.c.i"
	cd /home/anton/program/c/cmake/build/socket_bin/bin && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home/anton/program/c/cmake/socket/src/main/socket.c > CMakeFiles/socket.dir/main/socket.c.i

socket_bin/bin/CMakeFiles/socket.dir/main/socket.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/socket.dir/main/socket.c.s"
	cd /home/anton/program/c/cmake/build/socket_bin/bin && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home/anton/program/c/cmake/socket/src/main/socket.c -o CMakeFiles/socket.dir/main/socket.c.s

socket_bin/bin/CMakeFiles/socket.dir/main/socket.c.o.requires:
.PHONY : socket_bin/bin/CMakeFiles/socket.dir/main/socket.c.o.requires

socket_bin/bin/CMakeFiles/socket.dir/main/socket.c.o.provides: socket_bin/bin/CMakeFiles/socket.dir/main/socket.c.o.requires
	$(MAKE) -f socket_bin/bin/CMakeFiles/socket.dir/build.make socket_bin/bin/CMakeFiles/socket.dir/main/socket.c.o.provides.build
.PHONY : socket_bin/bin/CMakeFiles/socket.dir/main/socket.c.o.provides

socket_bin/bin/CMakeFiles/socket.dir/main/socket.c.o.provides.build: socket_bin/bin/CMakeFiles/socket.dir/main/socket.c.o

# Object files for target socket
socket_OBJECTS = \
"CMakeFiles/socket.dir/main/socket.c.o"

# External object files for target socket
socket_EXTERNAL_OBJECTS =

../socket/socket: socket_bin/bin/CMakeFiles/socket.dir/main/socket.c.o
../socket/socket: socket_bin/bin/CMakeFiles/socket.dir/build.make
../socket/socket: ../socket/src/lib/libsocket_app.so
../socket/socket: ../socket/src/lib/libsocket_arp.so
../socket/socket: ../socket/src/lib/libsocket_base.a
../socket/socket: ../socket/src/lib/libsocket_property.a
../socket/socket: socket_bin/bin/CMakeFiles/socket.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C executable ../../../socket/socket"
	cd /home/anton/program/c/cmake/build/socket_bin/bin && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/socket.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
socket_bin/bin/CMakeFiles/socket.dir/build: ../socket/socket
.PHONY : socket_bin/bin/CMakeFiles/socket.dir/build

socket_bin/bin/CMakeFiles/socket.dir/requires: socket_bin/bin/CMakeFiles/socket.dir/main/socket.c.o.requires
.PHONY : socket_bin/bin/CMakeFiles/socket.dir/requires

socket_bin/bin/CMakeFiles/socket.dir/clean:
	cd /home/anton/program/c/cmake/build/socket_bin/bin && $(CMAKE_COMMAND) -P CMakeFiles/socket.dir/cmake_clean.cmake
.PHONY : socket_bin/bin/CMakeFiles/socket.dir/clean

socket_bin/bin/CMakeFiles/socket.dir/depend:
	cd /home/anton/program/c/cmake/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/anton/program/c/cmake /home/anton/program/c/cmake/socket/src /home/anton/program/c/cmake/build /home/anton/program/c/cmake/build/socket_bin/bin /home/anton/program/c/cmake/build/socket_bin/bin/CMakeFiles/socket.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : socket_bin/bin/CMakeFiles/socket.dir/depend

