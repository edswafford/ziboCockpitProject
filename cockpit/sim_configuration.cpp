#include "sim_configuration.hpp"

//#include <Shlobj.h>
#include "yaml-cpp/yaml.h"
#include <fstream>
#include <ostream>
#include <cctype>
#include <algorithm>
#include <vector>
#include "logger.hpp"
#include "util.hpp"
#include "wx/log.h"
#include <filesystem>

namespace fs = std::filesystem;


extern logger LOG;


	static inline std::string& toUpper(std::string& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), ::toupper);
		return str;
	}

	SimConfiguration::SimConfiguration() : CockpitCfgAbstractBase()
	{
		fs::path default_path = fs::current_path().concat("SimConfig.yaml");

		fsDisplays_config_file = default_path.string();
		if (!Util::file_exists(fsDisplays_config_file))
		{
			char * homepath = getenv("homepath");
			if(homepath != nullptr) {
				fs::path home = "C:";
				auto documents = home.append(homepath).append("Documents").append("SimConfig.yaml");
				fsDisplays_config_file = documents.string();
				if(!Util::file_exists(fsDisplays_config_file)) {
					home = home.append("SimConfig.yaml");
					if(!Util::file_exists(fsDisplays_config_file)) {
						std::string message = "Cannot find Cockpit Configuration file: " + fsDisplays_config_file;
						throw FileNotFoundException(message);
					}
				}
			}
			else {
				std::string message = "Cannot find Cockpit Configuration file: Enviroment 'homepath is not defined'";
				throw FileNotFoundException(message);
			}
		}
		std::ifstream input{fsDisplays_config_file};
		std::ostringstream configBuf;
		char c;
		while(input.get(c))
		{
			configBuf << static_cast<unsigned char>(std::tolower(c));
		}

		YAML::Node base_node = YAML::Load(configBuf.str());

		eng1_min = base_node["eng1_min"].as<int>();
		eng2_min = base_node["eng2_min"].as<int>();
		spdbrk_min = base_node["spdbrk_min"].as<int>();
		rev1_min = base_node["rev1_min"].as<int>();
		rev2_min = base_node["rev2_min"].as<int>();

		eng1_max = base_node["eng1_max"].as<int>();
		eng2_max = base_node["eng2_max"].as<int>();
		spdbrk_max = base_node["spdbrk_max"].as<int>();
		rev1_max = base_node["rev1_max"].as<int>();
		rev2_max = base_node["rev2_max"].as<int>();

	}

	void SimConfiguration::saveConfig()
	{
		std::ofstream fout(fsDisplays_config_file);

		fout << "# Sim737 Configuration\n";
		fout << "#\n";
		fout << "eng1_min: " << eng1_min << "\n";
		fout << "eng2_min: " << eng2_min << "\n";
		fout << "spdbrk_min: " << spdbrk_min << "\n";
		fout << "rev1_min: " << rev1_min << "\n";
		fout << "rev2_min: " << rev2_min << "\n";

		fout << "eng1_max: " << eng1_max << "\n";
		fout << "eng2_max: " << eng2_max << "\n";
		fout << "spdbrk_max: " << spdbrk_max << "\n";
		fout << "rev1_max: " << rev1_max << "\n";
		fout << "rev2_max: " << rev2_max << "\n";


	}

	std::unique_ptr<CockpitCfgAbstractBase> CockpitCfg::ptr(nullptr);

	CockpitCfg::CockpitCfg(std::unique_ptr<CockpitCfgAbstractBase> cfgPrt)
	{
		assert( cfgPrt != nullptr );
		assert( ptr == nullptr );
		ptr = std::move(cfgPrt);
	}

	void CockpitCfg::save()
	{
		ptr->saveConfig();
	}

