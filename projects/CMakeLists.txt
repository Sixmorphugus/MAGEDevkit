# PROJECT MACROS
# find all subdirectories in the current directory
macro(SUBDIRLIST result curdir)
	file(GLOB children RELATIVE ${curdir} ${curdir}/*)
	set(dirlist "")
	foreach(child ${children})
		if(IS_DIRECTORY ${curdir}/${child})
			list(APPEND dirlist ${child})
		endif()
	endforeach()
	set(${result} ${dirlist})
endmacro()

macro(DECLARE_MAGEPROJECT name includeDir sourceFiles)
	add_executable(${name} ${sourceFiles} ${MAGE_SHARED})
	
	message(STATUS "MAGE project ${name} has source files ${sourceFiles}")
	
	include_directories(${includeDir})
	
	# required libs
	target_link_libraries(${name} ${MAGE_ALL_LIBS})
	include_directories(${MAGE_ALL_INCLUDES})

	# On any UNIX os, projects need libdl
	if(UNIX)
		target_link_libraries(${name} dl)
	endif()

	# copy resources if any are found
	if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data)
		add_custom_command(TARGET ${name} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data $<TARGET_FILE_DIR:${name}>)
	endif()
	
	# fix msc's working directory
	if(MSVC)
		file( WRITE "${CMAKE_CURRENT_BINARY_DIR}/(${name}.vcxproj.user" 
		"<?xml version=\"1.0\" encoding=\"utf-8\"?>
		<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">
		  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='RelWithDebInfo|x64'\">
			<LocalDebuggerWorkingDirectory>$(OutDir)</LocalDebuggerWorkingDirectory>
			<DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
		  </PropertyGroup>
		  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">
			<LocalDebuggerWorkingDirectory>$(OutDir)</LocalDebuggerWorkingDirectory>
			<DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
		  </PropertyGroup>
		  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='MinSizeRel|x64'\">
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
endmacro()

# add all subdirectories as projects
SUBDIRLIST(projects ${CMAKE_CURRENT_SOURCE_DIR})

FOREACH(pro ${projects})
  add_subdirectory(${pro})
ENDFOREACH()