#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(ConvertRootFiles)

#----------------------------------------------------------------------------
# Find ROOT
#
find_package(ROOT)
if(ROOT_FOUND)
    message("ROOT found")
    include(${ROOT_USE_FILE})
else()
    message("Error ROOT NOT FOUND")
endif()

#-------------------------------------------
# Locate sources and headers for this project
#
include_directories(${ROOT_INCLUDE_DIR})

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries

add_executable(ConvertRootFiles ConvertRootFiles.cpp)
target_link_libraries(ConvertRootFiles ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS ConvertRootFiles DESTINATION bin)
