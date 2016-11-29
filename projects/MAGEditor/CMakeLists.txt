# wxWidgets finding/using
find_package(wxWidgets COMPONENTS core base adv propgrid)

if(wxWidgets_FOUND)
	file(GLOB MAGEditor_SRC
		"${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
	)

	# Level editor
	DECLARE_MAGEPROJECT(MAGEditor "${CMAKE_CURRENT_SOURCE_DIR}/include" "${MAGEditor_SRC}")

	# On any UNIX os, MAGEditor needs X11 and GTK
	if(UNIX)
		target_link_libraries(MAGEditor X11)

		include_directories(${GTK2_INCLUDE_DIRS})
		target_link_libraries(MAGEditor ${GTK2_LIBRARIES})
	endif()

	# MAGEditor needs wxWidgets
	include(${wxWidgets_USE_FILE})
	target_link_libraries(MAGEditor ${wxWidgets_LIBRARIES})

	# copy dll files
	if(WIN32)
		add_custom_command(TARGET MAGEditor POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
			"${wxWidgets_ROOT_DIR}/lib/vc_dll/wxbase310u_vc_custom.dll"
			"$<TARGET_FILE_DIR:MAGEditor>/wxbase310u_vc_custom.dll"
		COMMAND ${CMAKE_COMMAND} -E copy
			"${wxWidgets_ROOT_DIR}/lib/vc_dll/wxmsw310u_core_vc_custom.dll"
			"$<TARGET_FILE_DIR:MAGEditor>/wxmsw310u_core_vc_custom.dll"
		COMMAND ${CMAKE_COMMAND} -E copy
			"${wxWidgets_ROOT_DIR}/lib/vc_dll/wxmsw310u_adv_vc_custom.dll"
			"$<TARGET_FILE_DIR:MAGEditor>/wxmsw310u_adv_vc_custom.dll"
		COMMAND ${CMAKE_COMMAND} -E copy
			"${wxWidgets_ROOT_DIR}/lib/vc_dll/wxmsw310u_propgrid_vc_custom.dll"
			"$<TARGET_FILE_DIR:MAGEditor>/wxmsw310u_propgrid_vc_custom.dll"
		)
	endif()
else()
	message(STATUS "No wxWidgets version was found that can be used to compile the editor, so the editor can't be compiled ):")
endif()