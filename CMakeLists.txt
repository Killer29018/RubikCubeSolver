cmake_minimum_required(VERSION 3.18)

project (RubikCube)

set (CMAKE_CXX_STANDARD 20)

# Import Libraries
find_package(OpenGL REQUIRED)

# Add Platform Specific Linker Options
set (linker "")
if (WIN32)
    set (linker "-lopengl32 -luser32 -limm32")
elseif(UNIX)
    set (linker "-ldl")
endif()

# Change Output Directory
set (CMAKE_BINARY_DIR "/bin")
set (OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}${CMAKE_BINARY_DIR}")
if (CMAKE_BUILD_TYPE MATCHES Debug)
    message ("Building Debug")
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}/debug")
    set (linker "${linker} -g")
    add_compile_definitions(DEBUG_MODE)
else ()
    message ("Building Release")
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}/release")
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unknown-pragmas")

file (GLOB_RECURSE cppFiles CONFIGURE_DEPENDS "src/*.cpp" "src/*.hpp")
add_executable(${PROJECT_NAME} ${cppFiles})

# Add Subdirectories
add_subdirectory(vendor)

# Add libraries and include directories
target_include_directories(${PROJECT_NAME} PUBLIC
    KRE
    vendor/glad/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${linker} PUBLIC
    glad
    OpenGL::GL
    glm::glm
    glfw
    Imgui

    KRE
)

