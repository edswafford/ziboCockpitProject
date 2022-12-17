cmake_minimum_required (VERSION 3.21 FATAL_ERROR)


option(ENABLE_UNIT_TESTS "Enable unit tests" ON)
message(STATUS "Enable testing: ${ENABLE_UNIT_TESTS}")

if (ENABLE_UNIT_TESTS)
    # Low Level Tests
    add_executable(low_level_test "")
    target_sources(low_level_test
	    PRIVATE
		    "low_level_tests/test_double_almost_equal.cpp"
            "low_level_tests/test_float_almost_equal.cpp"
            "low_level_tests/test_queue.cpp"
    )
    target_link_libraries(low_level_test
  	    PRIVATE
    	    gtest gtest_main
    )
    
    # Common Test
    add_executable(common_test "")
    target_sources(common_test
        PRIVATE
        "../../common/network/end_point.hpp"
        
        "mocks/socket_mock.cpp"
        "mocks/udp_mock.hpp"
        "common/test_multicast_controller.cpp"
        "common/test_packets.cpp"
       )
        target_link_libraries(common_test
  	    PUBLIC
    	    gtest gmock gtest_main zboCockpitCommon
    )

    # xplane plugin Tests
    add_executable(plugin_test "")
    target_sources(plugin_test
	    PRIVATE
            "cockpit/test_data_refs.cpp"
            "mocks/xplane_mock.cpp"
     "cockpit/test_server.cpp"
     )
    target_link_libraries(plugin_test
  	    PRIVATE
    	    gtest gmock gtest_main
    )
    if(APPLE)
        target_compile_definitions(plugin_test PUBLIC -DAPL=1 -DIBM=0 -DLIN=0)
    elseif(UNIX)
        target_compile_definitions(plugin_test PUBLIC -DAPL=0 -DIBM=0 -DLIN=1)
    elseif(WIN32)
        target_compile_definitions(plugin_test PUBLIC -DAPL=0 -DIBM=1 -DLIN=0)
    endif()
    target_compile_definitions(plugin_test PUBLIC -DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1 -DXPLM302=1 -DXPLM303=1)
    add_test(
  	    NAME LowLevel_test
  	    COMMAND $<TARGET_FILE:low_level_test>
    )
       add_test(
  	    NAME xplane_plugin_test
  	    COMMAND $<TARGET_FILE:plugin_test>
    )
       add_test(
  	    NAME common_test
  	    COMMAND $<TARGET_FILE:common_test>
    )
endif()
