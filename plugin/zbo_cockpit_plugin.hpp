#pragma once


#include <XPLMDataAccess.h>
#include <XPLMMenus.h>
#include "server.hpp"

namespace zcockpit::plugin
{

#define VERSION 0.3f



	class ZboCockpitPlugin {

	public:
		ZboCockpitPlugin() = delete;
		ZboCockpitPlugin(std::unique_ptr <zcockpit::plugin::Server> server) : server_(std::move(server)), flightLoopInterval_(1.0f / 30.f) {}

	public:

		// X-Plane API
		float flightLoop(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter, void* inRefcon) const;
		int pluginStart(char* out_name, char* out_sig, char* out_desc);
		void pluginStop() const;
		static void receiveMessage(XPLMPluginID inFromWho, long inMessage, void* inParam);

		static void menu_handler(void* in_menu_ref, void* in_item_ref);


	public: // DataRefProvider implementation



	private:
		float flightLoopInterval_; // Delay between loop calls (in seconds)

		int g_menu_container_idx; // The index of our menu item in the Plugins menu
		XPLMMenuID g_menu_id; // The menu container we'll append all our menu items to
		char msg[200];


		// Server initializes Network
		// All udp communication goes through the server
		std::unique_ptr <zcockpit::plugin::Server> server_;
	};
}