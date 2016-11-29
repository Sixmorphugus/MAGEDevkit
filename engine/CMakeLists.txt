cmake_minimum_required(VERSION 3.6)

######################
# COMPILE PROPERTIES

# all of this is contained in the MAGE project
project(MAGE)

# MAGE's files
file(GLOB MAGE_SRC
    "include/MAGE/*.h"
    "src/*.cpp"
)

# make everything output to the same place (we do not have to copy sfml's libs to where MAGE is if MAGE already outputs next to them)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

######################
# COMPILE DEPENDENCIES

add_subdirectory(extlibs) # find all externals

# options for externals - use all sfml libs but network
set(MAGE_SFML_LIB sfml-main sfml-system sfml-window sfml-graphics sfml-audio)
set(MAGE_SFML_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/SFML/include)

set(MAGE_CHAI_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/chaiscript)

######################
# COMPILE DATA

# Multipurpose Arcade Game Engine (Core)
add_library(MAGE SHARED ${MAGE_SRC})
include_directories("./include/MAGE")

target_compile_definitions(MAGE PRIVATE MAGEDLL_EXPORT) # tell MAGE's header files that they are exporting functions, not importing them

# set flags for the engine to compile properly
if(MSVC)
	SET(CMAKE_CXX_FLAGS "/EHsc /bigobj")
elseif(CMAKE_COMPILER_IS_GNUCC)
	SET(CMAKE_CXX_FLAGS "-std=c++0x")
endif()

# Link libraries - include required files
target_link_libraries(MAGE ${MAGE_SFML_LIB})
include_directories(${MAGE_SFML_INCLUDE})

include_directories(${MAGE_CHAI_INCLUDE}) # chai is header only

# On any UNIX os, MAGE needs libdl
if(UNIX)
	target_link_libraries(MAGE dl)
endif()

######################
# POST-COMPILE

# update the version number upon each compilation
add_custom_command(
    TARGET MAGE
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -DPROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}" -DPROJECT_BINARY_DIR="${PROJECT_BINARY_DIR}" -P "${CMAKE_CURRENT_LIST_DIR}/version.cmake"
)

######################
# PRE-COMPILE

# get the initial git version number (sadly this repeats code)

# find git version number
execute_process(
  COMMAND git rev-list HEAD --count
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT GIT_VERSION)
	set(GIT_VERSION 0)
endif()

# build.h will therefore contain our build number accurately enough to use
configure_file (
  "${PROJECT_SOURCE_DIR}/build.h.in"
  "${PROJECT_BINARY_DIR}/build.h"
)

# ----

include_directories("${PROJECT_BINARY_DIR}") # reason for this is that build.h is created in the engine's compilation folder

# fix msc's working directory
if(MSVC)
	file( WRITE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.vcxproj.user" 
	"<?xml version=\"1.0\" encoding=\"utf-8\"?>
	<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">
	  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">
		<LocalDebuggerWorkingDirectory>$(OutDir)</LocalDebuggerWorkingDirectory>
		<DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
	  </PropertyGroup>
	  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\">
		<LocalDebuggerWorkingDirectory>$(OutDir)</LocalDebuggerWorkingDirectory>
		<DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
	  </PropertyGroup>
	  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='RelWithDebInfo|Win32'\">
		<LocalDebuggerWorkingDirectory>$(OutDir)</LocalDebuggerWorkingDirectory>
		<DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
	  </PropertyGroup>
	  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">
		<LocalDebuggerWorkingDirectory>$(OutDir)</LocalDebuggerWorkingDirectory>
		<DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
	  </PropertyGroup>
	  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='MinSizeRel|Win32'\">
		<LocalDebuggerWorkingDirectory>$(OutDir)</LocalDebuggerWorkingDirectory>
		<DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
	  </PropertyGroup>
	  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">
		<LocalDebuggerWorkingDirectory>$(OutDir)</LocalDebuggerWorkingDirectory>
		<DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
	  </PropertyGroup>
	</Project>")
endif()