#pragma once

#include <string>
#include "../../aircraft_model.hpp"
#include "iocards.hpp"

namespace zcockpit::cockpit::hardware
{
	class MipIOCard : public IOCards
	{
	public:
		MipIOCard() = delete;
		explicit MipIOCard(AircraftModel& ac_model, const std::string& device_bus_addr);

		[[nodiscard]] static std::unique_ptr<MipIOCard> create_iocard(AircraftModel& ac_model, const std::string& bus_address);

		void processMIP();
		void updateRelays(int state);

		static bool is_running(){return running;}
		static std::string get_bus_addr(){return iocard_bus_addr;}
	private:

		AircraftModel& aircraft_model;
		void processOutputs();

		static std::string iocard_bus_addr;
		static bool running;


		// Capt Main Panel DUs
		int captOutboardPFD;
		int captNormal;
		int captEngPrimary;
		int captPFD;

		// Capt Lower DU
		int captEngPrimaryLowerDU;
		int captPFDLowerDU;

		// Capt Main Panel DUs
		int foOutboardPFD;
		int foNormal;
		int foEngPrimary;
		int foPFD;
		int foMFD;

		// FO Lower DU
		int foEngPrimaryLowerDU;
		int foPFDLowerDU;
		int foNDLowerDU;

		int foClock;
		int FoMasterFire;
		int FoMasteCaution;
		int warningAnn;
		int eng2N2Set;
		int eng1N2Set;
		int N1SetAuto;
		int N1SetBoth;
		int apWarning;
		int atWarning;
		int fmcWarning;
		int afdsTest2;
		int afdsTest1;
		int gpws;
		int flapsInhibit;
		int terrainInhibit;
		int gearInhibit;
		int rudderLeft;
		int rudderRight;
		int aileronLeft;
		int aileronRight;
	};
}
