#pragma once

#include <string>
#include "../../aircraft_model.hpp"
#include "iocards.hpp"

namespace zcockpit::cockpit::hardware
{
	class MipIOCard : public IOCards
	{
	public:
		static constexpr int MIP_SW_SIZE = 72;

		MipIOCard() = delete;
		explicit MipIOCard(AircraftModel& ac_model, const std::string& device_bus_addr);

		[[nodiscard]] static std::unique_ptr<MipIOCard> create_iocard(AircraftModel& ac_model, const std::string& bus_address);
		void processEncoders();

		void processMIP();
		void updateRelays(int state);

		static bool is_running(){return running;}
		static std::string get_bus_addr(){return iocard_bus_addr;}
	private:
		void initialize_switches();

		AircraftModel& aircraft_model;
		static std::string iocard_bus_addr;
		static bool running;

		ZcockpitSwitch iocard_mip_zcockpit_switches[MIP_SW_SIZE]{};


		// Capt Main Panel DUs
		int captOutboardPFD{0};
		int captNormal{0};
		int captEngPrimary{0};
		int captPFD{0};

		// Capt Lower DU
		int captEngPrimaryLowerDU{0};
		int captPFDLowerDU{0};

		// Capt Main Panel DUs
		int foOutboardPFD{0};
		int foNormal{0};
		int foEngPrimary{0};
		int foPFD{0};
		int foMFD{0};

		// FO Lower DU
		int foEngPrimaryLowerDU{0};
		int foPFDLowerDU{0};
		int foNDLowerDU{0};

		int foClock{0};
		int FoMasterFire{0};
		int FoMasteCaution{0};
		int warningAnn{0};
		int eng2N2Set{0};
		int eng1N2Set{0};
		int N1SetAuto{0};
		int N1SetBoth{0};
		int apWarning{0};
		int atWarning{0};
		int fmcWarning{0};
		int afdsTest2{0};
		int afdsTest1{0};
		int gpws{0};
		int flapsInhibit{0};
		int terrainInhibit{0};
		int gearInhibit{0};
		int rudderLeft{0};
		int rudderRight{0};
		int aileronLeft{0};
		int aileronRight{0};
	};
}
