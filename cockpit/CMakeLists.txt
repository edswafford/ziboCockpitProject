
cmake_minimum_required (VERSION 3.24 FATAL_ERROR)


# Add source to this project's executable.
add_executable (client WIN32
	"gui/zcockpitApp.cpp"
	"gui/wxfb/mainApp.cpp"
	"gui/zcockpitApp.hpp"


	"client.h"
#	"main.cpp"
	"../common/xp_data_ref_repository.hpp" 
	"aircraft_model.hpp"
	"aircraft_model.cpp"
	"aircraft_parameters.hpp"

	
	"hardware/sim737_hardware.hpp"
	"hardware/sim737_hardware.cpp"

	"hardware/InterfaceIT/interfaceit.cpp" 
	"hardware/InterfaceIT/interfaceit.hpp"  
	"hardware/InterfaceIT/HidInterfaceIT.cpp" 
	"hardware/InterfaceIT/HidInterfaceIT.hpp"
	"hardware/InterfaceIT/interfaceITAPI_Data.h"
	  
	

	
	"hardware/usb/hidapi.c" 
	"hardware/usb/hidapi.h" 
	"hardware/usb/usbrelay.cpp"

    "hardware/health.hpp" 
	#"websocket/websocket_client.hpp"
	#"websocket/websocket_client.cpp"

	"aircraft_model_init_data.cpp"
	"aircraft_model_init_str.cpp"   
  
	"../common/util.cpp"

	"hardware/usb/libusb_interface.cpp" 
	"hardware/ioCards/iocards.cpp" 
	"hardware/ioCards/iocards.hpp"	
	"hardware/ioCards/mip_iocard.cpp"
	"hardware/ioCards/fwd_overhead_iocard.cpp"
	"hardware/ioCards/rear_overhead_iocard.cpp"

	"hardware/ftd2xx/ftd2xxdevices_wrapper.cpp"
	"hardware/flight_illusion/fidevice.cpp"	
	"hardware/flight_illusion/ficontroller.cpp"

	 "hardware/transponder/transponder.cpp"
	
	 "hardware/throttle/throttle_joystick.cpp"
	 "hardware/throttle/Pokeys/stepper.cpp"
	 "hardware/throttle/Pokeys/PokeysLib.cpp"
	 "hardware/throttle/Vjoy/VjoyFeeder.cpp"

	 "${CMAKE_SOURCE_DIR}/third_party/iir_lib/iir/Butterworth.cpp"
	 "${CMAKE_SOURCE_DIR}/third_party/iir_lib/iir/ChebyshevI.cpp"
	 "${CMAKE_SOURCE_DIR}/third_party/iir_lib/iir/ChebyshevII.cpp"
	 "${CMAKE_SOURCE_DIR}/third_party/iir_lib/iir/Cascade.cpp"

 	 "${CMAKE_SOURCE_DIR}/third_party/iir_lib/iir/PoleFilter.cpp"
	 "${CMAKE_SOURCE_DIR}/third_party/iir_lib/iir/RBJ.cpp"
	 "${CMAKE_SOURCE_DIR}/third_party/iir_lib/iir/Biquad.cpp"
	 "${CMAKE_SOURCE_DIR}/third_party/iir_lib/iir/Custom.cpp"

	 "sim_configuration.cpp"
	"gui/mainWindow.cpp"
)



target_compile_definitions(client PRIVATE _UNICODE=1)
target_compile_definitions(client PRIVATE FTD2XX_STATIC=1)
# target_compile_definitions(client PRIVATE WIN32_LEAN_AND_MEAN=1)
# target_compile_definitions(client PRIVATE LOG4CXX_STATIC=1)
# target_compile_definitions(client PRIVATE _WIN32_WINNT=0x0601)
# target_compile_definitions(client PRIVATE _WINDOWS=1)
# target_compile_definitions(client PRIVATE WIN32=1)
# 

target_include_directories(client PRIVATE ${wxWidgets_SOURCE_DIR}/include/msvc)
target_include_directories(client PRIVATE ${wxWidgets_SOURCE_DIR}/include)
target_include_directories(client PRIVATE ${CMAKE_SOURCE_DIR}/third_party/Vjoy)
target_include_directories(client PRIVATE ${wxWidgets_SOURCE_DIR}/include)
target_include_directories(client PRIVATE ${CMAKE_SOURCE_DIR}/third_party/iir_lib)

target_link_directories(client PRIVATE ${CMAKE_BINARY_DIR}/lib/vc_x64_lib/)

set(FTD2XX_STATIC_LIBRARY "${CMAKE_SOURCE_DIR}/third_party/FTD2XX/Static/amd64")
set(VJOY_STATIC_LIBRARY "${CMAKE_SOURCE_DIR}/third_party/Vjoy/lib")

target_include_directories(client PRIVATE ${CMAKE_SOURCE_DIR}/third_party/yaml-cpp/include/)





 # Link required libraries to the executable
target_link_libraries(client PUBLIC
		zboCockpitCommon
		libusb
		yaml-cpp
		${FTD2XX_STATIC_LIBRARY}/ftd2xx.lib
		${VJOY_STATIC_LIBRARY}/vJoyInterface.lib
		asio
        wxbase
        wxcore
        wxnet
        wxxml
)


add_custom_command(TARGET client POST_BUILD        # Adds a post-build event to client
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${CMAKE_SOURCE_DIR}/third_party/Vjoy/lib/vJoyInterface.dll"      # <--this is in-file
        $<TARGET_FILE_DIR:client>)                 # <--this is out-file path

