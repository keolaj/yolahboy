# CMakeList.txt : CMake project for yolahboy, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("yolahboy")

#find_library(SDL2, "C:\\Users\\Keola\\dev\\lib\\SDL2-2.30.8\\lib\\x64")

# Add source to this project's executable.
add_executable (yolahboy "yolahboy.c" "yolahboy.h" "components/cpu2.h" "components/cpu2.c"  "components/emulator.h" "components/emulator.c" "components/memory2.h" "components/memory2.c" "components/operation_defitions.h" "components/operations.h" "components/operations.c" "components/cpu_definitions.h" "components/gpu2.h" "components/gpu2.c" "components/memory_definitions.h" "components/controller.h" "components/controller.c")
target_include_directories(yolahboy PUBLIC "C:\\Users\\Keola\\dev\\lib\\SDL2-2.30.8\\include")
target_link_libraries(yolahboy "C:\\Users\\Keola\\dev\\lib\\SDL2-2.30.8\\lib\\x64\\SDL2.lib" "C:\\Users\\Keola\\dev\\lib\\SDL2-2.30.8\\lib\\x64\\SDL2main.lib")
set_property(TARGET yolahboy PROPERTY CXX_STANDARD 20)

# TODO: Add tests and install targets if needed.
