# default cmake_minimum_required
cmake_minimum_required (VERSION 3.6)

if(NOT IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/engine)
	message(FATAL_ERROR "The engine submodule is missing. Did you forget to git clone with --recursive??")
endif()

# Enable debug symbols by default
# must be done before project() statement
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()
# (you can also set it on the command line: -D CMAKE_BUILD_TYPE=Release)

project(MAGEDev)

# load the engine
add_subdirectory(engine)

# index all the shared files
file(GLOB MAGE_SHARED
    "shared/src/*.cpp"
	"shared/include/*.h"
)

set(MAGE_SHARED_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/shared/include)

# options for compiler (if you're working with an engine that uses SFML types, you need SFML)
set(MAGE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/engine)

set(MAGE_INCLUDE ${MAGE_DIR}/include)

set(MAGE_SFML_LIB sfml-main sfml-system sfml-window sfml-graphics sfml-audio)
set(MAGE_SFML_INCLUDE ${MAGE_DIR}/extlibs/SFML/include)

set(MAGE_CHAI_INCLUDE ${MAGE_DIR}/extlibs/chaiscript) # required for registering things to the scripting engine

set(MAGE_ALL_LIBS ${MAGE_SFML_LIB} MAGE)
set(MAGE_ALL_INCLUDES ${MAGE_SFML_INCLUDE} ${MAGE_INCLUDE} ${MAGE_CHAI_INCLUDE} ${MAGE_SHARED_INCLUDE} ${PROJECT_BINARY_DIR}/engine)

# make everything go into a bin folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# load our projects
add_subdirectory(projects)
