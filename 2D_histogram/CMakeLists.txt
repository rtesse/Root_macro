#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(2D_histogram)

#----------------------------------------------------------------------------
# Setup ROOT include directories and compile definitions
#
find_package(ROOT REQUIRED)
include(${ROOT_USE_FILE})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include
                    ${ROOT_INCLUDE_DIR})
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(2D_histogram 2D_histogram.cpp ${sources} ${headers})
target_link_libraries(2D_histogram ${ROOT_LIBRARIES} )

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS 2D_histogram DESTINATION bin)
