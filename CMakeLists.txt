cmake_minimum_required(VERSION 3.16)
project(CricketManager VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(OpenGL REQUIRED)
find_package(CURL REQUIRED)

# Find nlohmann/json (header-only library)
find_package(nlohmann_json 3.2.0 REQUIRED)

# Find GLM (OpenGL Mathematics)
find_package(GLM REQUIRED)

# Find GLEW (OpenGL Extension Wrangler)
find_package(GLEW REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/imgui)
include_directories(/opt/homebrew/include)

# Source files
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Player.cpp
    src/Team.cpp
    src/MatchEngine.cpp
    src/AuctionManager.cpp
    src/CommentaryManager.cpp
    src/GUIManager.cpp
    src/AnimationHandler.cpp
    src/Venue.cpp
    src/DataManager.cpp
    src/MatchVisualizer.cpp
    src/Shader.cpp
    src/Model.cpp
    src/ParticleSystem.cpp
    src/ScreenEffect.cpp
    third_party/imgui/imgui.cpp
    third_party/imgui/imgui_demo.cpp
    third_party/imgui/imgui_draw.cpp
    third_party/imgui/imgui_tables.cpp
    third_party/imgui/imgui_widgets.cpp
    third_party/imgui/imgui_impl_sdl2.cpp
    third_party/imgui/imgui_impl_opengl3.cpp
)

# Create executable
add_executable(CricketManager ${SOURCES})

# Link libraries
target_link_libraries(CricketManager
    ${SDL2_LIBRARIES}
    ${OPENGL_LIBRARIES}
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
    SDL2_mixer::SDL2_mixer
    CURL::libcurl
    nlohmann_json::nlohmann_json
    GLEW::GLEW
    GLM::GLM
)

# Include directories for libraries
target_include_directories(CricketManager PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
)

# Copy assets to build directory
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})

# Platform-specific settings
if(APPLE)
    target_link_libraries(CricketManager "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
    set_target_properties(CricketManager PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/Info.plist
    )
elseif(UNIX)
    target_compile_options(CricketManager PRIVATE -Wall -Wextra)
elseif(WIN32)
    target_compile_options(CricketManager PRIVATE /W4)
endif()

# Installation
install(TARGETS CricketManager
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)

# Print configuration info
message(STATUS "Cricket Manager Configuration:")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  SDL2: ${SDL2_VERSION}")
message(STATUS "  OpenGL: Found")
message(STATUS "  GLEW: Found")
message(STATUS "  GLM: Found")
message(STATUS "  CURL: Found")
message(STATUS "  nlohmann/json: Found")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}") 