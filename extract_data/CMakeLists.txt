#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(Extract_data)

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
include_directories(${PROJECT_SOURCE_DIR}/include
                    ${ROOT_INCLUDE_DIRS})

link_directories(${ROOT_INCLUDE_DIRS})

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries

add_executable(Extract_data Extract_data.cc ${sources} ${headers})
target_link_libraries(Extract_data ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS Extract_data DESTINATION bin)
