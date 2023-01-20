#pragma once

class CockpitCfgAbstractBase
{
public:
	CockpitCfgAbstractBase()
	{
	}

	virtual ~CockpitCfgAbstractBase()
	{
	}

	virtual void saveConfig() = 0;

	unsigned long eng1_min{ 0 };
	unsigned long eng2_min{ 0 };
	unsigned long spdbrk_min{ 0 };
	unsigned long rev1_min{ 0 };
	unsigned long rev2_min{ 0 };
	unsigned long eng1_max{ 0 };
	unsigned long eng2_max{ 0 };
	unsigned long spdbrk_max{ 0 };
	unsigned long rev1_max{ 0 };
	unsigned long rev2_max{ 0 };

};
