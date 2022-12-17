

# To build shared libraries in Windows, we set CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS to TRUE.
# See https://cmake.org/cmake/help/v3.4/variable/CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS.html
# See https://blog.kitware.com/create-dlls-on-windows-without-declspec-using-new-cmake-export-all-feature/
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(library_name zbo_cockpit_plugin)
set(FILES 
	"plugin_api.cpp"
	"zbo_cockpit_plugin.cpp"
	"zbo_cockpit_plugin.hpp" 
	"server.hpp"
	"server.cpp"
	"../common/xp_data_ref.hpp"
	"../common/xp_data_ref.cpp"
	"../common/xp_command.hpp"
	"../common/xp_command.cpp"
	"../common/xp_command_repository.hpp"
	"../common/xp_command_repository.cpp"	
)   

message(STATUS "Building Plugin")
message(STATUS "Xplane path: $ENV{XPLANE_PATH}")

if(APPLE)
    add_executable(zbo_cockpit_plugin ${FILES})
elseif(UNIX)
    add_library(zbo_cockpit_plugin SHARED ${FILES})
elseif(WIN32)
    add_library(zbo_cockpit_plugin MODULE ${FILES} )
    target_compile_definitions(zbo_cockpit_plugin PUBLIC -D_CRT_SECURE_NO_WARNINGS /bigobj)
endif()


if(APPLE)
    set(XPLANE_LIBRARY_PATH "${CMAKE_SOURCE_DIR}/third_party/SDK/Libraries/Mac/")
elseif(WIN32)
    set(XPLANE_LIBRARY_PATH "${CMAKE_SOURCE_DIR}/third_party/SDK/Libraries/Win/")
endif()

set(XPLANE_INCLUDES_PATH "${CMAKE_SOURCE_DIR}/third_party/SDK/CHeaders/" CACHE STRING PATH)

if(APPLE)
	message(STATUS apple target made)
	find_library(XPLM_LIBRARY XPLM "${XPLANE_LIBRARY_PATH}")
	find_library(XPWIDGETS_LIBRARY XPWidgets "${XPLANE_LIBRARY_PATH}")
	add_library(xplm STATIC IMPORTED GLOBAL)
	add_library(xpwidgets STATIC IMPORTED GLOBAL)
	set_property(TARGET xplm PROPERTY IMPORTED_LOCATION "${XPLM_LIBRARY}")
	set_property(TARGET xpwidgets PROPERTY IMPORTED_LOCATION "${XPWIDGETS_LIBRARY}")
elseif(WIN32)
	message(STATUS "windows target made")
	find_library(XPLM_LIBRARY XPLM_64 "${XPLANE_LIBRARY_PATH}")
	find_library(XPWIDGETS_LIBRARY XPWidgets_64 "${XPLANE_LIBRARY_PATH}")
	add_library(xplm SHARED IMPORTED GLOBAL)
	add_library(xpwidgets SHARED IMPORTED GLOBAL)
	set_property(TARGET xplm PROPERTY IMPORTED_IMPLIB "${XPLM_LIBRARY}")
	set_property(TARGET xpwidgets PROPERTY IMPORTED_IMPLIB "${XPWIDGETS_LIBRARY}")
elseif(UNIX)
	message(STATUS linux target made)
	add_library(xplm INTERFACE)
	add_library(xpwidgets INTERFACE)
endif()



if(APPLE)
    target_compile_definitions(zbo_cockpit_plugin PUBLIC -DAPL=1 -DIBM=0 -DLIN=0)
elseif(UNIX)
    target_compile_definitions(zbo_cockpit_plugin PUBLIC -DAPL=0 -DIBM=0 -DLIN=1)
elseif(WIN32)
    # TARGET_COMPILE_OPTIONS(zbo_cockpit_plugin PRIVATE "/MT$<$<CONFIG:Debug>:d>")
    target_compile_definitions(zbo_cockpit_plugin PUBLIC -DAPL=0 -DIBM=1 -DLIN=0)
endif()


target_compile_definitions(zbo_cockpit_plugin PUBLIC -DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1 -DXPLM302=1 -DXPLM303=1)

set_target_properties(zbo_cockpit_plugin PROPERTIES OUTPUT_NAME "zCockpit.xpl")


set_target_properties(zbo_cockpit_plugin PROPERTIES PREFIX "")
set_target_properties(zbo_cockpit_plugin PROPERTIES SUFFIX "")

#link flags
if(APPLE)
    set_property(TARGET zbo_cockpit_plugin APPEND_STRING PROPERTY LINK_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fPIC -fvisibility=hidden -bundle")
elseif(UNIX)
    set_property(TARGET zbo_cockpit_plugin APPEND_STRING PROPERTY LINK_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -rdynamic -nodefaultlibs -undefined_warning -fPIC -fvisibility=hidden")
endif()

        

target_link_libraries(zbo_cockpit_plugin PUBLIC ${XPLM_LIBRARY} ${XPWIDGETS_LIBRARY} zboCockpitCommon)
target_include_directories(zbo_cockpit_plugin PUBLIC "${XPLANE_INCLUDES_PATH}/XPLM" "$ENV{XPLANE_INCLUDES_PATH}/Widgets" "${XPLANE_INCLUDES_PATH}/Wrappers")



add_custom_command(TARGET zbo_cockpit_plugin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/zCockpit.xpl.pdb "$ENV{XPLANE_PATH}/Resources/plugins/zCockpit/win_x64/zCockpit.pdb"
	COMMENT "Copying zCockpit.pdb to $ENV{XPLANE_PATH}/Resources/plugins/zCockpit/win_x64"
	VERBATIM 
)
add_custom_command(TARGET zbo_cockpit_plugin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/zCockpit.xpl "$ENV{XPLANE_PATH}/Resources/plugins/zCockpit/win_x64"
	 COMMENT "Copying zCockpit.xpl to $ENV{XPLANE_PATH}/Resources/plugins/zCockpit/win_x64"
	 VERBATIM 
)