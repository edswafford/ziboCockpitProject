
cmake_minimum_required (VERSION 3.24 FATAL_ERROR)


add_library(zboCockpitCommon 
	STATIC
	"network/udp.cpp"
	"logger.cpp"
	"packet.cpp"
	"shared_types.hpp"
 )