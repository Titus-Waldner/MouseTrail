cmake_minimum_required(VERSION 3.10)

project(MouseTrail LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect all .cpp files inside src/
file(GLOB SRC_FILES src/*.cpp)

# Include the resource file (app.rc) from the resources folder.
set(RESOURCE_FILES resources/app.rc)

# Create the WIN32 executable (no console on Windows) and include resource file.
add_executable(MouseTrail WIN32 ${SRC_FILES} ${RESOURCE_FILES})

# Include the header directory
target_include_directories(MouseTrail PRIVATE include)

# Link to the standard Windows libraries
target_link_libraries(MouseTrail PRIVATE user32 gdi32)

# For MinGW/Clang, ensure a Windows subsystem and static linking
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_link_options(MouseTrail PRIVATE -mwindows -static -static-libgcc -static-libstdc++)
endif()

# For MSVC, specify a Windows subsystem
if(MSVC)
    set_target_properties(MouseTrail PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:WINDOWS"
    )
endif()

#if (MSVC)
#    # No /SUBSYSTEM:WINDOWS so the console window remains open
#    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
#elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
#    # Static linking to avoid missing DLLs, but no -mwindows so console is kept
#    target_link_options(${PROJECT_NAME} PRIVATE -static -static-libgcc -static-libstdc++)
#endif()
