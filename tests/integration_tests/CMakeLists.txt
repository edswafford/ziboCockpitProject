cmake_minimum_required (VERSION 3.21 FATAL_ERROR)

add_executable (integration_test_server 
	"multicast_server.cpp" 
	"../../plugin/server.cpp"
	"../../common/xp_data_ref.hpp"
	"../../common/xp_data_ref.cpp"
	"../../common/xp_command.hpp"
	"../../common/xp_command.cpp"
	"../../common/xp_command_repository.hpp"
	"../../common/xp_command_repository.cpp"
)



if(APPLE)
    target_compile_definitions(integration_test_server PUBLIC -DAPL=1 -DIBM=0 -DLIN=0)
elseif(UNIX)
    target_compile_definitions(integration_test_server PUBLIC -DAPL=0 -DIBM=0 -DLIN=1)
elseif(WIN32)
    # TARGET_COMPILE_OPTIONS(zbo_cockpit_plugin PRIVATE "/MT$<$<CONFIG:Debug>:d>")
    target_compile_definitions(integration_test_server PUBLIC -DAPL=0 -DIBM=1 -DLIN=0)
endif()


target_link_libraries(integration_test_server PUBLIC zboCockpitCommon PUBLIC ${XPLM_LIBRARY} )