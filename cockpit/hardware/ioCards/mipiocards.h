#ifndef MIPIOCARDS_H
#define MIPIOCARDS_H

#include <string>
#include "iocards.h"

namespace zcockpit::cockpit::hardware
{
	class MIPioCards : public IOCards
	{
	public:
		MIPioCards(std::string deviceBusAddr);


		void processMIP();
		void updateRelays(int state);

	private:

		void processOutputs();
	;

		bool powerIsOn;

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



#endif // MIPIOCARDS_H
