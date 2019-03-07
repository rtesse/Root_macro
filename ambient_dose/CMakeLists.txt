#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(Ambient_dose)

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


#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries

add_executable(Ambient_dose Ambient_dose.cc)
target_link_librariesAmbient_dose ${ROOT_LIBRARIES} GSL::gsl GSL::gslcblas)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS Ambient_dose DESTINATION bin)
# file(COPY CrossSection DESTINATION .)
