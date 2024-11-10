# CMakeList.txt : CMake project for yolahboy, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# set(VCPKG_TARGET_TRIPLET "x64-windows")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE")

project ("yolahboy")

find_package(SDL3 REQUIRED)

add_definitions("-DIMGUI_USER_CONFIG=\"../cimconfig.h\"")
add_definitions("-DIMGUI_DISABLE_OBSOLETE_FUNCTIONS=1")

add_compile_definitions("IMGUI_IMPL_SDL3")


set(BACKENDS_FOLDER "./cimgui/imgui/backends")
message(STATUS "backends folder: " ${BACKENDS_FOLDER})
set(IMGUI_PATH "./cimgui/imgui")
set(IMGUI_SOURCES 

./cimgui/cimgui.cpp
./cimgui/imgui/imgui.cpp
./cimgui/imgui/imgui_draw.cpp
./cimgui/imgui/imgui_demo.cpp
./cimgui/imgui/imgui_widgets.cpp
./cimgui/imgui/imgui_tables.cpp

)

if (WIN32)
    add_definitions("-DIMGUI_IMPL_API=extern \"C\" __declspec\(dllexport\)")
else(WIN32)
    add_definitions("-DIMGUI_IMPL_API=extern \"C\" ")
endif(WIN32)

set(IMGUI_SOURCES_sdl)
set(IMGUI_LIBRARIES)

list(APPEND IMGUI_SOURCES ${BACKENDS_FOLDER}/imgui_impl_sdlrenderer3.cpp ${BACKENDS_FOLDER}/imgui_impl_sdl3.cpp)

add_library(cimgui_sdl SHARED ${IMGUI_SOURCES})
target_link_libraries(cimgui_sdl SDL3::SDL3)
target_include_directories(cimgui_sdl PUBLIC "./cimgui/imgui")

# Add source to this project's executable.
add_executable (yolahboy "yolahboy.c" "yolahboy.h" "components/cpu2.h" "components/cpu2.c"  "components/emulator.h" "components/emulator.c" "components/memory2.h" "components/memory2.c" "components/operation_defitions.h" "components/operations.h" "components/operations.c" "components/cpu_definitions.h" "components/gpu2.h" "components/gpu2.c" "components/memory_definitions.h" "components/controller.h" "components/controller.c" "debugger/debugger.c" "debugger/debugger.h" "emulator_main.c" "emulator_main.h")
target_include_directories(yolahboy PUBLIC "./cimgui/generator/output" "./cimgui")
target_link_libraries(yolahboy PUBLIC SDL3::SDL3 cimgui_sdl)
set_property(TARGET yolahboy PROPERTY CXX_STANDARD 20)

# TODO: Add tests and install targets if needed.
