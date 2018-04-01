# Set project name
project(Game)
# Define target name
set(TARGET_NAME Game)
set(CMAKE_BUILD_TYPE Debug)

######################################

# Set CMake minimum version and CMake policy required by Urho3D-CMake-common module
if (WIN32)
    cmake_minimum_required(VERSION 3.2.3)      # Going forward all platforms will use this as minimum version
else ()
    cmake_minimum_required(VERSION 2.8.6)
endif ()
if (COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
    if (CMAKE_VERSION VERSION_GREATER 2.8.12 OR CMAKE_VERSION VERSION_EQUAL 2.8.12)
        # INTERFACE_LINK_LIBRARIES defines the link interface
        cmake_policy(SET CMP0022 NEW)
    endif ()
    if (CMAKE_VERSION VERSION_GREATER 3.0.0 OR CMAKE_VERSION VERSION_EQUAL 3.0.0)
        # Disallow use of the LOCATION target property - so we set to OLD as we still need it
        cmake_policy(SET CMP0026 OLD)
        # MACOSX_RPATH is enabled by default
        cmake_policy(SET CMP0042 NEW)
    endif ()
endif ()
# Set CMake modules search path
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)
# Include Urho3D Cmake common module
include(UrhoCommon)

file(GLOB BMP EasyBMP_1.06/*.cpp EasyBMP_1.06/*.h)
file(GLOB ANALYZER ImageAnalyzer/*.cpp ImageAnalyzer/*.h)
file(GLOB TRAINING Training/*.cpp Training/*.h)
file(GLOB GAME GameSource/*.cpp GameSource/*.h)
file(GLOB LINALG LinAlg/linalg.h)
file(GLOB TEST Tests/*.h Tests/*.cpp)


#setup game files
set(SOURCE_FILES ${GAME} ${ANALYZER} ${BMP} ${LINALG})
# Setup target with resource copying
setup_main_executable()
target_link_libraries(Game fann)

#setup training files
set(TRAINING_SOURCE_FILES ${TRAINING} ${ANALYZER} ${BMP})
add_executable(Training ${TRAINING_SOURCE_FILES})
target_link_libraries(Training fann)

#setup test files
set(TEST_SOURCE_FILES ${TEST} ${ANALYZER} ${BMP})
add_executable(Tests ${TEST_SOURCE_FILES})
target_link_libraries(Tests fann)
