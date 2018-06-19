#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(1DPlotting)

#----------------------------------------------------------------------------
# Find ROOT (required package)
#
list(APPEND CMAKE_PREFIX_PATH /home/rtesse/Software/ROOT/root-build/)
find_package(ROOT REQUIRED COMPONENTS RIO Net)
include(${ROOT_USE_FILE})


#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${ROOT_INCLUDE_DIR})

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(1DPlotting 1DPlotting.cpp)
target_link_libraries(1DPlotting ${ROOT_LIBRARIES} )


#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS 1DPlotting DESTINATION bin)

