
cmake_minimum_required (VERSION 3.24 FATAL_ERROR)


# Add source to this project's executable.
add_executable (client WIN32
	"gui/zcockpitApp.cpp"
	"gui/wxfb/mainApp.cpp"
	"gui/zcockpitApp.h"


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
  
	"hardware/util.cpp"

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
target_link_directories(client PRIVATE ${CMAKE_BINARY_DIR}/lib/vc_x64_lib/)

set(FTD2XX_STATIC_LIBRARY "${CMAKE_SOURCE_DIR}/third_party/FTD2XX/Static/amd64")



 # Link required libraries to the executable
target_link_libraries(client PUBLIC
		zboCockpitCommon
		libusb
		${FTD2XX_STATIC_LIBRARY}/ftd2xx.lib
		asio
        wxbase
        wxcore
        wxnet
        wxxml
)



