cmake_minimum_required(VERSION 3.10)
project(SmartHomeAutomation)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directory
include_directories(include)

# Find source files
file(GLOB SOURCES "src/*.cpp")

# Create executable
add_executable(SmartHome ${SOURCES})

# Add threading library
find_package(Threads REQUIRED)
target_link_libraries(SmartHome Threads::Threads)