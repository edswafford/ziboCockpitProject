#pragma once
#include <string>
#include <memory>
#include <iostream>
#include <cassert>
#include "sim_configuration_base.hpp"


class SimConfiguration : public CockpitCfgAbstractBase
	{
	public:
		SimConfiguration();

		~SimConfiguration()
		{
			std::cout << "SimConfiguration destroy\n";
		}

		void saveConfig() override;
	private:
		std::string fsDisplays_config_file;



	};


	class CockpitCfg
	{
	public:
		explicit CockpitCfg(std::unique_ptr<CockpitCfgAbstractBase> cfgPtr);

		CockpitCfg()
		{
			std::cout << "CockpitCfg()\n";
			assert( ptr != nullptr );
		}

		~CockpitCfg()
		{
			std::cout << "CockpitCfg destroy\n";
		}

		static void save();

		CockpitCfg(CockpitCfg const&) = delete; // Copy construct
		CockpitCfg(CockpitCfg&&) = delete; // Move construct

		static std::unique_ptr<CockpitCfgAbstractBase> ptr;

	private:
	};

