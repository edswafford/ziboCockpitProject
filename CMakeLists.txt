# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.24 FATAL_ERROR)

project ("zcockpit_projects" VERSION 1.0.1)

# Project uses latest standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Using the Asio library without Boost
add_definitions(-DASIO_STANDALONE)

include(FetchContent)

# Link against pthreads under Unix systems
if (NOT MSVC AND NOT MINGW)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()
# Force WebSocket++ to use 100% C++11 mode, so that it doesn't try to look for Boost
# (Note that under MinGW, Boost.Thread is still required, due to a bug in MinGW that prevents the C++11 version from being used)
add_definitions(-D_WEBSOCKETPP_CPP11_STRICT_)

FetchContent_Declare(asio
  GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
  GIT_TAG asio-1-22-1
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  )
message(STATUS "ASIO populated ${asio_POPULATED}")

if(NOT asio_POPULATED)
FetchContent_MakeAvailable(asio)
endif()

add_library(asio INTERFACE)
target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)
find_package(Threads)
target_link_libraries(asio INTERFACE Threads::Threads)



# ------------------ wxWidgets ------------------
    set(wxBUILD_SHARED OFF CACHE BOOL "Build wx libraries as shared libs")
    set(wxBUILD_PRECOMP OFF CACHE BOOL "Use precompiled headers")
    set(wxBUILD_MONOLITHIC OFF CACHE BOOL "Build a single library")
    
    FetchContent_Declare(
        wxWidgets 
        GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
        GIT_TAG        v3.2.1
        GIT_SHALLOW     TRUE
    )
    
    FetchContent_GetProperties(wxWidgets)
    FetchContent_MakeAvailable(wxWidgets)
    


#FetchContent_Declare(websocketpp
#  GIT_REPOSITORY  https://github.com/zaphoyd/websocketpp.git
#  GIT_TAG 0.8.2
#  CONFIGURE_COMMAND ""
#  BUILD_COMMAND ""
#  ) 
#message(STATUS "Websockerpp populated ${websocketpp_POPULATED}")

#if(NOT websocketpp_POPULATED)
#FetchContent_MakeAvailable(websocketpp)
#endif()

#message(STATUS "Websocketpp Source ${websocketpp_SOURCE_DIR}")
#include_directories(${websocketpp_SOURCE_DIR})


#FetchContent_Declare(json
#  GIT_REPOSITORY  https://github.com/nlohmann/json.git
#  GIT_TAG v3.10.5
#  CONFIGURE_COMMAND ""
#  BUILD_COMMAND ""
#  )
#message(STATUS "Json populated ${json_POPULATED}")

#if(NOT json_POPULATED)
#	FetchContent_MakeAvailable(json)
#endif()
#include_directories(${json_SOURCE_DIR}/single_include/nlohmann)
#message(STATUS "JSON Source ${json_SOURCE_DIR}")


FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


include_directories(third_party/SDK/CHeaders/XPLM)
include_directories(third_party/cereal/include)
include_directories(third_party/libusb-1.0.26/libusb)
include_directories(third_party/libusb-1.0.26/libusb/os)
include_directories(third_party/FTD2XX/)


add_subdirectory(third_party/libusb-1.0.26/libusb)

add_executable(test_usb third_party/libusb-1.0.26/examples/testlibusb.c)
target_link_libraries (test_usb PUBLIC libusb)


include_directories(common)

# Include sub-projects.
add_subdirectory(common)
add_subdirectory (cockpit)
add_subdirectory (plugin)



##add_subdirectory(tests/integration_tests)


# TESTING
enable_testing()
##add_subdirectory(tests/unit_tests)
