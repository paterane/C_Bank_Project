# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.26

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = D:\CClass\NCC_CLASS\Bank_Project

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\CClass\NCC_CLASS\Bank_Project\build

# Include any dependencies generated for this target.
include CMakeFiles/bnkfunc.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/bnkfunc.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/bnkfunc.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/bnkfunc.dir/flags.make

CMakeFiles/bnkfunc.dir/include/bankHandle.c.obj: CMakeFiles/bnkfunc.dir/flags.make
CMakeFiles/bnkfunc.dir/include/bankHandle.c.obj: D:/CClass/NCC_CLASS/Bank_Project/include/bankHandle.c
CMakeFiles/bnkfunc.dir/include/bankHandle.c.obj: CMakeFiles/bnkfunc.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\CClass\NCC_CLASS\Bank_Project\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/bnkfunc.dir/include/bankHandle.c.obj"
	C:\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/bnkfunc.dir/include/bankHandle.c.obj -MF CMakeFiles\bnkfunc.dir\include\bankHandle.c.obj.d -o CMakeFiles\bnkfunc.dir\include\bankHandle.c.obj -c D:\CClass\NCC_CLASS\Bank_Project\include\bankHandle.c

CMakeFiles/bnkfunc.dir/include/bankHandle.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/bnkfunc.dir/include/bankHandle.c.i"
	C:\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\CClass\NCC_CLASS\Bank_Project\include\bankHandle.c > CMakeFiles\bnkfunc.dir\include\bankHandle.c.i

CMakeFiles/bnkfunc.dir/include/bankHandle.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/bnkfunc.dir/include/bankHandle.c.s"
	C:\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S D:\CClass\NCC_CLASS\Bank_Project\include\bankHandle.c -o CMakeFiles\bnkfunc.dir\include\bankHandle.c.s

CMakeFiles/bnkfunc.dir/include/utils.c.obj: CMakeFiles/bnkfunc.dir/flags.make
CMakeFiles/bnkfunc.dir/include/utils.c.obj: D:/CClass/NCC_CLASS/Bank_Project/include/utils.c
CMakeFiles/bnkfunc.dir/include/utils.c.obj: CMakeFiles/bnkfunc.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\CClass\NCC_CLASS\Bank_Project\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/bnkfunc.dir/include/utils.c.obj"
	C:\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/bnkfunc.dir/include/utils.c.obj -MF CMakeFiles\bnkfunc.dir\include\utils.c.obj.d -o CMakeFiles\bnkfunc.dir\include\utils.c.obj -c D:\CClass\NCC_CLASS\Bank_Project\include\utils.c

CMakeFiles/bnkfunc.dir/include/utils.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/bnkfunc.dir/include/utils.c.i"
	C:\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\CClass\NCC_CLASS\Bank_Project\include\utils.c > CMakeFiles\bnkfunc.dir\include\utils.c.i

CMakeFiles/bnkfunc.dir/include/utils.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/bnkfunc.dir/include/utils.c.s"
	C:\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S D:\CClass\NCC_CLASS\Bank_Project\include\utils.c -o CMakeFiles\bnkfunc.dir\include\utils.c.s

CMakeFiles/bnkfunc.dir/include/encrypt_decrypt.c.obj: CMakeFiles/bnkfunc.dir/flags.make
CMakeFiles/bnkfunc.dir/include/encrypt_decrypt.c.obj: D:/CClass/NCC_CLASS/Bank_Project/include/encrypt_decrypt.c
CMakeFiles/bnkfunc.dir/include/encrypt_decrypt.c.obj: CMakeFiles/bnkfunc.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\CClass\NCC_CLASS\Bank_Project\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/bnkfunc.dir/include/encrypt_decrypt.c.obj"
	C:\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/bnkfunc.dir/include/encrypt_decrypt.c.obj -MF CMakeFiles\bnkfunc.dir\include\encrypt_decrypt.c.obj.d -o CMakeFiles\bnkfunc.dir\include\encrypt_decrypt.c.obj -c D:\CClass\NCC_CLASS\Bank_Project\include\encrypt_decrypt.c

CMakeFiles/bnkfunc.dir/include/encrypt_decrypt.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/bnkfunc.dir/include/encrypt_decrypt.c.i"
	C:\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\CClass\NCC_CLASS\Bank_Project\include\encrypt_decrypt.c > CMakeFiles\bnkfunc.dir\include\encrypt_decrypt.c.i

CMakeFiles/bnkfunc.dir/include/encrypt_decrypt.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/bnkfunc.dir/include/encrypt_decrypt.c.s"
	C:\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S D:\CClass\NCC_CLASS\Bank_Project\include\encrypt_decrypt.c -o CMakeFiles\bnkfunc.dir\include\encrypt_decrypt.c.s

# Object files for target bnkfunc
bnkfunc_OBJECTS = \
"CMakeFiles/bnkfunc.dir/include/bankHandle.c.obj" \
"CMakeFiles/bnkfunc.dir/include/utils.c.obj" \
"CMakeFiles/bnkfunc.dir/include/encrypt_decrypt.c.obj"

# External object files for target bnkfunc
bnkfunc_EXTERNAL_OBJECTS =

D:/CClass/NCC_CLASS/Bank_Project/lib/libbnkfunc.a: CMakeFiles/bnkfunc.dir/include/bankHandle.c.obj
D:/CClass/NCC_CLASS/Bank_Project/lib/libbnkfunc.a: CMakeFiles/bnkfunc.dir/include/utils.c.obj
D:/CClass/NCC_CLASS/Bank_Project/lib/libbnkfunc.a: CMakeFiles/bnkfunc.dir/include/encrypt_decrypt.c.obj
D:/CClass/NCC_CLASS/Bank_Project/lib/libbnkfunc.a: CMakeFiles/bnkfunc.dir/build.make
D:/CClass/NCC_CLASS/Bank_Project/lib/libbnkfunc.a: CMakeFiles/bnkfunc.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\CClass\NCC_CLASS\Bank_Project\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking C static library D:\CClass\NCC_CLASS\Bank_Project\lib\libbnkfunc.a"
	$(CMAKE_COMMAND) -P CMakeFiles\bnkfunc.dir\cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\bnkfunc.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/bnkfunc.dir/build: D:/CClass/NCC_CLASS/Bank_Project/lib/libbnkfunc.a
.PHONY : CMakeFiles/bnkfunc.dir/build

CMakeFiles/bnkfunc.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\bnkfunc.dir\cmake_clean.cmake
.PHONY : CMakeFiles/bnkfunc.dir/clean

CMakeFiles/bnkfunc.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\CClass\NCC_CLASS\Bank_Project D:\CClass\NCC_CLASS\Bank_Project D:\CClass\NCC_CLASS\Bank_Project\build D:\CClass\NCC_CLASS\Bank_Project\build D:\CClass\NCC_CLASS\Bank_Project\build\CMakeFiles\bnkfunc.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/bnkfunc.dir/depend

