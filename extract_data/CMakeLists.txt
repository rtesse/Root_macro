#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

set(source_file Extract_data.cc)
set(Project_name Extract_data)

project(${Project_name})

# Before launch Cmake, add the root environnement for CLion

# https://stackoverflow.com/questions/37662130/clion-or-cmake-does-not-see-environment-variable

#list(APPEND CMAKE_PREFIX_PATH /home/rtesse/Software/ROOT/root-buid/)
#list(APPEND CMAKE_PREFIX_PATH "C:\root_v6.14.04\cmake")

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
message(STATUS $ENV{ROOTSYS})

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


#----------------------------------------------------------------------------
# Find GSL
#
find_package(GSL REQUIRED)    # See below (2)

#-------------------------------------------
# Locate sources and headers for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include
        ${ROOT_INCLUDE_DIRS})

link_directories(${ROOT_INCLUDE_DIRS})

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries

add_executable(${Project_name} ${source_file} ${sources} ${headers})
target_link_libraries(${Project_name} ${ROOT_LIBRARIES} GSL::gsl GSL::gslcblas)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS ${Project_name} DESTINATION bin)
