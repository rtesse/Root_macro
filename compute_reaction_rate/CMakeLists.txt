#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(ComputeReactionRate)

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

find_package(GSL REQUIRED)    # See below (2)

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

add_executable(ComputeReactionRate ComputeReactionRate.cc ${sources} ${headers})
target_link_libraries(ComputeReactionRate ${ROOT_LIBRARIES} GSL::gsl GSL::gslcblas)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS ComputeReactionRate DESTINATION bin)
file(COPY CrossSection DESTINATION .)
