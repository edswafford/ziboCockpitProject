
#include "zcockpitApp.h"
#include <wx/log.h> 
#include "../websocket/websocket_client.hpp"
#include <chrono>

#include "../client.h"
#include <cstdlib>
#include <iostream>
#include <thread>


#include "logger.hpp"


#include "network/connection.hpp"
#include "network/multicast_controller.hpp"
#include "network/udp.hpp"
#include "network/udp_controller.hpp"
#include "../hardware/InterfaceIT/HidInterfaceIT.hpp"

#include <cassert>
#include "xplane_status.cpp"
logger LOG("ZiboCockpit Client.log");



#include "../hardware/usb/libusb_interface.hpp"

#include "../hardware/sim737_hardware.hpp"
//#include "../hardware/ioCards/mip_iocard.hpp"
#include "../hardware/ioCards/fwd_overhead_iocard.hpp"
//#include "../hardware/ioCards/rear_overhead_iocard.hpp"

using namespace std::chrono_literals;
using namespace zcockpit::common::network;
using namespace zcockpit::cockpit::hardware;


namespace zcockpit::cockpit::gui
{

	ZcockpitApp::ZcockpitApp()
	{

		// Start with everything timed out
		current_cycle = ONE_SECOND;
		five_hz_counter = FIVE_HZ;
		ten_hz_counter = TEN_HZ;
		twenty_hz_counter = TWENTY_HZ;
		five_second_counter = FIVE_SECONDS;


	}

	bool ZcockpitApp::OnInit()
	{
		LOG() << "Starting Main Window";

		// call the base class initialization method, currently it only parses a
		// few common command-line options but it could be do more in the future
		if (!wxApp::OnInit()) {
			return false;
		}

		
		main_window = new MainWindow(nullptr);
		main_window->Show(true);

		//
		// Simple Ini
		ini.SetUnicode();
		SI_Error ini_error = ini.LoadFile("sim737.ini");
		if(ini_error != SI_OK) {
			ini_error = ini.LoadFile("../../../../sim737.ini");
		}
		if(ini_error == SI_OK)
		{
			// Nothing right now!!
		}
		main_window->set_iocard_mip_status(false);
		main_window->set_iocard_fwd_overhead_status(false);
		main_window->set_iocard_rear_overhead_status(false);

		//
		// Initialize USB Library
		if(LibUsbInterface::initialize()) {
			main_window->set_iocard_status(IOCards::devices);
		}
		else {
			main_window->set_iocard_status(std::string("Failed to Initialize LibUsb"));
		}

		timer_thread = std::thread([this]
		{
		   this->timer();
		});
		return true;
	}

	int ZcockpitApp::OnExit() {
		// Any custom deletion here
		// DON'T call delete m_frame or m_frame->Destroy() m_frame here
		// As it is null at this point.

		// now we can stop the update
		LOG() << "Exiting Main Window";

		std::unique_lock<std::mutex> lk(timer_done_mutex);
		{
			std::lock_guard<std::mutex> lock(sim737_timer_mutex);
			stop_timer = true;
		}
		condition.wait(lk, [this]
			{
				return (this->timer_has_stopped);
			});
		if(timer_thread.joinable()) {
			timer_thread.join();
		}

		LibUsbInterface::exit();

		return 0;
	}

	void ZcockpitApp::update_counters()
	{
		static int sleep_adjust = 0;
		elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);

		// sleep
		long long wait = UPDATE_RATE - elapsed.count() - sleep_adjust;

		if(wait > 0)
		{
			auto sleep_time = wait;
			std::this_thread::sleep_for(std::chrono::milliseconds(wait));
			auto now = std::chrono::high_resolution_clock::now();
			elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
			sleep_adjust = (elapsed.count() - UPDATE_RATE) / 2;
			if(sleep_adjust > 10 || sleep_adjust < -10){
				LOG() << "After Sleep, Update rate " << UPDATE_RATE <<  "ms sleep time " << sleep_time << "ms elasped time since cycle start " << elapsed.count() << "ms sleep adjust " << sleep_adjust << "ms";
			}
		}
		else
		{
			LOG() << "Cycle slip Elapsed: " << elapsed.count() << " wait : " << wait;
		}

		// update counters
		if(current_cycle >= ONE_SECOND)
		{
			current_cycle = 0;
		}
		else
		{
			current_cycle++;
		}
		if(five_second_counter >= FIVE_SECONDS)
		{
			five_second_counter = 0;
		}
		else
		{
			five_second_counter++;
		}


		if(twenty_hz_counter > TWENTY_HZ)
		{
			twenty_hz_counter = 0;
		}
		else
		{
			twenty_hz_counter++;
		}


		if(ten_hz_counter > TEN_HZ)
		{
			ten_hz_counter = 0;
		}
		else
		{
			ten_hz_counter++;
		}
		if(five_hz_counter >= FIVE_HZ)
		{
			five_hz_counter = 0;
		}
		else
		{
			five_hz_counter++;
		}
	}

	void ZcockpitApp::timer()
	{
			try {
			// random number generator
			std::random_device rd; // Will be used to obtain a seed for the random number engine
			std::seed_seq seed{ rd(), rd(), rd(), rd() };
			std::mt19937 gen(seed); // Standard mersenne_twister_engine seeded with rd()


			// Creating an Udp object will initialize the Network
			// and the destructor will do the Network cleanup
			// which is only needed for Windows OS 
			Udp udp{};

			Connection<Udp> multicast_server_connection{ udp, 0, 0 };

			const auto server_group_ip = udp.string_to_ip(Udp::kMulticastServerGroupIP);
			if (server_group_ip == 0) {
				wxSafeShowMessage("Fatal Error", "Cannot create server group ip address ... terminating");
				main_window->Close(true);
				return;
			}
			const auto app_group_ip = udp.string_to_ip(Udp::kMulticastAppGroupIP);
			if (app_group_ip == 0) {
				wxSafeShowMessage("Fatal Error", "Cannot create app group ip address ... terminating");
				main_window->Close(true);
				return;
			}

			// Create Receive ==  Server TX port
			if (!multicast_server_connection.
				create_receiver(Udp::kServerMulticastTxPortAddress, true)) {
				wxSafeShowMessage("Fatal Error", "Cannot create Receiver Port ... terminating");
				main_window->Close(true);
				return;
			}
			// Create Client->Server Transmitter  == Server RX Port
			if (!multicast_server_connection.create_transmitter(server_group_ip, Udp::kServerMulticastRxPortAddress)) {
				wxSafeShowMessage("Fatal Error", "Cannot create Transmitter Port to Server ... terminating");
				main_window->Close(true);
				return;
			}
			//
			// App connection used by the App 
			// so a websocket can be established
			Connection<Udp> multicast_app_connection{ udp, 0, 0 };

			// Create Receive ==  App TX port
			// Once we receive the App IP we can open our Client Websocket
			if (!multicast_app_connection.
				create_receiver(Udp::kAppMulticastTxPortAddress, true)) {
				wxSafeShowMessage("Fatal Error", "Cannot create Receiver Port ... terminating");
				main_window->Close(true);;
				return;
			}

			const std::string ip_address = udp.get_ip();
			if(ip_address.empty()) {
				auto message = "Cannot continue execution when my IP address cannot be found"; 
				LOG(Severe) << message;
				wxSafeShowMessage("Fatal Error", message);
				main_window->Close(true);
				return;
			}
			auto multicast_server_controller = MulticastController(ip_address, udp, multicast_server_connection, gen);
			auto multicast_app_controller = MulticastController(ip_address, udp, multicast_app_connection, gen);
			auto udp_controller = UdpController<Udp>();

			auto connection = Connection<Udp>(udp, 0, 0);
			zcockpit::cockpit::AircraftModel aircraft_model;
			auto hid = std::make_unique<HidInterfaceIT>();
			zcockpit::cockpit::hardware::InterfaceIT interface_it(aircraft_model, std::move(hid));

			zcockpit::cockpit::Client<Udp> client(ip_address, udp, multicast_server_controller, multicast_app_controller, udp_controller, connection, gen, aircraft_model, UPDATE_RATE);

			zcockpit::cockpit::hardware::Sim737Hardware sim737_hardware(aircraft_model, interface_it);
			
			// IOCards should be initialized by now
			//
			main_window->set_iocard_status(IOCards::devices);
			main_window->set_iocard_mip_addr(MipIOCard::get_bus_addr());
			main_window->set_iocard_fwd_overhead_addr(ForwardOverheadIOCard::get_bus_addr());
			main_window->set_iocard_rear_overhead_addr(RearOverheadIOCard::get_bus_addr());

			LOG() << "Client running";

			// #################################################################
			//
			//**************** NOT USING WEBSOCKETS YET *********************
			//
			// #################################################################

			//
			//	WEBSOCKET
			//
	////		WebsocketClientEndpoint client_endpoint;
	////		int websocket_id = client_endpoint.connect("ws://192.168.0.143:8080");
		
		/*	std::string input;
			int id = 0;
			char key = ' ';
			int message_number = 0;

			*/

			// TODO Do we need to know if local xplane is running
	////		XplaneStatus::is_xplane_local();


			stop_timer = false;
			timer_has_stopped = false;


			while (true)
			{
				start = std::chrono::high_resolution_clock::now();

				// do we need to shut down
				{
					std::lock_guard<std::mutex> lock(sim737_timer_mutex);
					if (stop_timer)
					{
						break;
					}
				}


				client.update(current_cycle);
				auto current_zibo_status = client.ziboStatus();
				if((zibo_status != current_zibo_status) || current_cycle == ONE_SECOND) {
					zibo_status = current_zibo_status;
					main_window->set_zibo_status(zibo_status);
				}
//				if(zibo_status)
				{

					sim737_hardware.do_updates(current_cycle);
				}


				if (current_cycle >= ONE_SECOND)
				{
					sim737_hardware.checkConnections();
				
					//
					// InterfaceIT MIP
					auto current_interfaceit_mip_status = sim737_hardware.interfaceitMipStatus();
					if(interfaceit_mip_status != current_interfaceit_mip_status || current_cycle == ONE_SECOND) {
						interfaceit_mip_status = current_interfaceit_mip_status;
						main_window->set_interfaceit_mip_status(interfaceit_mip_status);
					}
					//
					// Interfaceit Overhead
					auto current_interfaceit_overhead_status = sim737_hardware.interfaceitOverheadStatus();
					if(interfaceit_overhead_status != current_interfaceit_overhead_status || current_cycle == ONE_SECOND) {
						interfaceit_overhead_status = current_interfaceit_overhead_status;
						main_window->set_interfaceit_overhead_status(interfaceit_overhead_status);
					}

					//
					// IOCards MIP
					auto current_iocard_mip_status = sim737_hardware.get_iocard_mip_status();
					if(iocard_mip_status != current_iocard_mip_status || current_cycle == ONE_SECOND) {
						iocard_mip_status = current_iocard_mip_status;
						main_window->set_iocard_mip_status(iocard_mip_status);
					}
					//
					// IOCards Forward Overhead
					auto current_iocard_forward_overhead_status = sim737_hardware.get_iocard_forward_overhead_status();
					if(iocard_forward_overhead_status != current_iocard_forward_overhead_status || current_cycle == ONE_SECOND) {
						iocard_forward_overhead_status = current_iocard_forward_overhead_status;
						main_window->set_iocard_fwd_overhead_status(iocard_forward_overhead_status);
					}
					//
					// IOCards Rear Overhead
					auto current_iocard_rear_overhead_status = sim737_hardware.get_iocard_rear_overhead_status();
					if(iocard_rear_overhead_status != current_iocard_rear_overhead_status || current_cycle == ONE_SECOND) {
						iocard_rear_overhead_status = current_iocard_rear_overhead_status;
						main_window->set_iocard_rear_overhead_status(iocard_rear_overhead_status);
					}
					//
					// Flight Illusion MIP
					auto current_flight_illusion_mip_status = sim737_hardware.get_flight_illusion_mip_status();
					if(flight_illusion_mip_status != current_flight_illusion_mip_status || current_cycle == ONE_SECOND) {
						flight_illusion_mip_status = current_flight_illusion_mip_status;
						main_window->set_flight_illusion_mip_status(flight_illusion_mip_status);
					}
					//
					// Flight Illusion Overhead
					auto current_flight_illusion_overhead_status = sim737_hardware.get_flight_illusion_overhead_status();
					if(flight_illusion_overhead_status != current_flight_illusion_overhead_status || current_cycle == ONE_SECOND) {
						flight_illusion_overhead_status = current_flight_illusion_overhead_status;
						main_window->set_flight_illusion_overhead_status(flight_illusion_overhead_status);
					}
					//
					// Flight Illusion MIP
					auto current_transponder_status = sim737_hardware.get_transponder_status();
					if(transponder_status != current_transponder_status || current_cycle == ONE_SECOND) {
						transponder_status = current_transponder_status;
						main_window->set_transponder_status(transponder_status);
					}
					






					// WEBSOCKET
					//
					////if(status_interval >= 20){
					////	status_interval = 0;
					////	id = client_endpoint.get_connection_id();
					////	if(id != 0) {
					////		json payload = {
					////			{"id", id},
					////			{"messageType", "status"},
					////			{"ipAddress", client.get_ip_address()},
					////			{"clientType", "hardware"},
					////			{"xplaneStatus", client.xplaneStatus()},
					////			{"ziboStatus", client.ziboStatus()},
					////			{"messageNumber", message_number++}
					////		};
					////		client_endpoint.send(payload.dump());
					////	}
					////}

				}

				update_counters();
			}




	////		json terminate = {{"id", id},
	////						{"messageType", "terminate"},};
	////		client_endpoint.send(terminate.dump());
	////		client_endpoint.close(websocketpp::close::status::normal, "Client Terminated");
	////		std::cout << "\nKeyboard Input Terminating ...\n";


			}
		////catch (websocketpp::exception const& e) {
		////	std::cout << e.what() << std::endl;
		////}
		catch (std::runtime_error& e) {
			wxSafeShowMessage("Fatal Error", "other exception ");
			main_window->Close(true);
		}


		//if (ovrheadIOCards != nullptr) {
		//	ovrheadIOCards->drop();
		//}

		LOG() << "Timer Stoped";

		std::unique_lock<std::mutex> lk(timer_done_mutex);
		timer_has_stopped = true;
		lk.unlock();
		condition.notify_one();

	}

//
// Entry int WxWidgets main()
//
wxIMPLEMENT_APP(ZcockpitApp);

}