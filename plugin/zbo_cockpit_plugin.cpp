#include "zbo_cockpit_plugin.hpp"
#include "logger.hpp"

#include <XPLMUtilities.h>



extern logger LOG;

namespace zcockpit::plugin
{


    static XPLMDataRef zbo_cockpit_plugin_version = nullptr;
    float get_zbo_cockpit_plugin_version(void* in_refcon);



    float ZboCockpitPlugin::flightLoop(float inElapsedSinceLastCall,
        float inElapsedTimeSinceLastFlightLoop,
        int inCounter,
        void* inRefcon) const
    {

        // Do processing
        server_->update();

        return flightLoopInterval_;
    }



    int ZboCockpitPlugin::pluginStart(char* out_name, char* out_sig, char* out_desc) {
        LOG() << "zboCockpit Plugin started";

        std::strcpy(out_name, "ZBO Cockpit");
        std::strcpy(out_sig, "open.source.zboCockpitplugin");
        std::strcpy(out_desc, "This plugin supports hardware (switches. button, lights ...) for Zibo 738");


        g_menu_container_idx = XPLMAppendMenuItem(XPLMFindPluginsMenu(), "ZBO Cockpit", 0, 0);
        g_menu_id = XPLMCreateMenu("ZBO Cockpit", XPLMFindPluginsMenu(), g_menu_container_idx, &ZboCockpitPlugin::menu_handler, NULL);

       std::sprintf(msg, "ZBO Cockpit Plugin Version: %0.1f", VERSION);
        XPLMAppendMenuItem(g_menu_id, msg, nullptr, 1);
        XPLMEnableMenuItem(g_menu_id, 0, 0);
        XPLMAppendMenuItem(g_menu_id, "Flush Log file", (void*)"Flush Log file", 1);
        XPLMAppendMenuSeparator(g_menu_id);
        XPLMAppendMenuItem(g_menu_id, "Toggle Shortcuts", (void*)"Menu Item 2", 1);
        XPLMAppendMenuItemWithCommand(g_menu_id, "Toggle Flight Configuration (Command-Based)", XPLMFindCommand("sim/operation/toggle_flight_config"));
        ;

      if (XPLMMenuID aircraft_menu = XPLMFindAircraftMenu()) // This will be NULL unless this plugin was loaded with an aircraft (i.e., it was located in the current aircraft's "plugins" subdirectory)
        {
            XPLMAppendMenuItemWithCommand(aircraft_menu, "Toggle Settings (Command-Based)", XPLMFindCommand("sim/operation/toggle_settings_window"));
        }

        // Init application and server


        //register datarefs

        zbo_cockpit_plugin_version = XPLMRegisterDataAccessor("zbocockpit/plugin/version",
            xplmType_Float,
            0,
            nullptr, nullptr,
            get_zbo_cockpit_plugin_version, nullptr,
            nullptr, nullptr,
            nullptr, nullptr,
            nullptr, nullptr,
            nullptr, nullptr,
            nullptr, nullptr);

        //init dataref

        zbo_cockpit_plugin_version = XPLMFindDataRef("zcockpit/plugin/version");
        XPLMSetDataf(zbo_cockpit_plugin_version, VERSION);

        // Log that we have started
        std::sprintf(msg, "ZBO Cockpit: Plugin Version: %0.1f started\n", VERSION);

        XPLMDebugString(msg);

        return 1;
    }


    void ZboCockpitPlugin::pluginStop() const
    {
        LOG() << "server closed";

        // Since we created this menu, we'll be good citizens and clean it up as well
        XPLMDestroyMenu(g_menu_id);
        // If we were able to add a command to the aircraft menu, it will be automatically removed for us when we're unloaded

        XPLMUnregisterDataAccessor(zbo_cockpit_plugin_version);
    }

    void ZboCockpitPlugin::receiveMessage(XPLMPluginID inFromWho, long inMessage, void* inParam) {
        LOG() << "Received Message From " << inFromWho  << " : " << inMessage;
    }

    void  ZboCockpitPlugin::menu_handler(void* in_menu_ref, void* in_item_ref)
    {
        if (!strcmp(static_cast<const char*>(in_item_ref), "Flush Log file"))
        {
            LOG() << "Flushing log file";
            LOG.flush();
        }
        else if (!strcmp(static_cast<const char*>(in_item_ref), "Menu Item 2"))
        {
            XPLMCommandOnce(XPLMFindCommand("sim/operation/toggle_key_shortcuts_window"));
        }
    }




    float get_zbo_cockpit_plugin_version(void* in_refcon) {
        return VERSION;
    }
}