#pragma once
#include "inc/public.h"
#include "inc/vjoyinterface.h"

enum class Vjoy_State
{
	None,
	Failed_To_Enable,
	Already_Owned_By_Another_Feeder,
	Disabled,
	Unknown_Status,
	Failed_To_Aquire,
	Valid,
};
struct Axis
{
	long axisX;
	long axisY;
	long axisZ;
	long axisR;
	long axisS;
	long axisT;
	long axisU;
	long axisV;

	//const long MAX_AXIS = 35000;
};

namespace  zcockpit::cockpit::hardware
{
	// Default device ID (Used when ID not specified)
	constexpr unsigned DEV_ID = 1;

	class VjoyFeeder
	{
	public:
		VjoyFeeder() = default;
		~VjoyFeeder();

		/*
		 * Return State: Aquired if success
		 */
		Vjoy_State init_vjoy(const unsigned id = DEV_ID);
		
		/*
		 * Return true if success
		 */
		bool update(const Axis& axis);

		bool update(const Axis& axis, const LONG lButtons);
		bool aquire() const;

		unsigned get_id() const
		{
			return dev_id;
		}
	private:
		unsigned dev_id{ DEV_ID };


	private:
		JOYSTICK_POSITION_V2 vjoy_data;
		Vjoy_State state{ Vjoy_State::None };
	};
}