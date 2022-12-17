#include <shlobj_core.h>

#include "../../common/logger.hpp"

extern logger LOG;

class XplaneStatus
{
public:
	static void is_xplane_local()
	{
		char szPath[MAX_PATH];

		if (SUCCEEDED(SHGetFolderPathA(NULL,
		   CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE,
		   NULL,
		   0,
		   szPath)))
		{
			auto xplane_install = std::string(szPath) + "\\x-plane_install_11.txt";
			LOG() << xplane_install;
		}
	}
};