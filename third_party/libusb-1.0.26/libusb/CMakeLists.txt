cmake_minimum_required (VERSION 3.24 FATAL_ERROR)

add_definitions(-DHAVE_STRUCT_TIMESPEC)

add_library(libusb STATIC 
	"config.h"	
	"core.c"
	"descriptor.c"
	"hotplug.c"
	"hotplug.h"	
	"io.c"
	"libusb.h"
	"libusbi.h"
	"strerror.c"
	"sync.c"
	"version.h"
	"version_nano.h"

	"os/events_windows.c"
	"os/events_windows.h" 
	"os/threads_windows.h"
	"os/threads_windows.c"
	"os/windows_common.c" 
	"os/windows_common.h"
	"os/windows_usbdk.c"
	"os/windows_usbdk.h"
	"os/windows_winusb.c"
	"os/windows_winusb.h"
)