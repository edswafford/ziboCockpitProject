
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
    "hardware/health.hpp" 
	#"websocket/websocket_client.hpp"
	#"websocket/websocket_client.cpp"

  "aircraft_model_init_data.cpp"
  "aircraft_model_init_str.cpp"   
  
  "hardware/util.cpp"

 # "hardware/ioCards/usbworker.cpp" 
 # "hardware/ioCards/usbworker.h" 
  "hardware/ioCards/iocards.cpp" 
  "hardware/ioCards/iocards.h"	
 # "hardware/ioCards/mipiocards.cpp" 
 # "hardware/ioCards/mipiocards.h"
  "hardware/ioCards/ovrheadiocards.cpp"
  "hardware/ioCards/ovrheadiocards.h"
 # "hardware/ioCards/rearovrheadiocards.cpp"
 # "hardware/ioCards/rearovrheadiocards.h"

 "hardware/usb/libusb_interface.cpp")



target_compile_definitions(client PRIVATE _UNICODE=1)

target_include_directories(client PRIVATE ${wxWidgets_SOURCE_DIR}/include/msvc)
target_include_directories(client PRIVATE ${wxWidgets_SOURCE_DIR}/include)
target_link_directories(client PRIVATE ${CMAKE_BINARY_DIR}/lib/vc_x64_lib/)




 # Link required libraries to the executable
target_link_libraries(client PUBLIC
		zboCockpitCommon
		libusb
		asio
        wxbase
        wxcore
        wxnet
        wxxml
)



