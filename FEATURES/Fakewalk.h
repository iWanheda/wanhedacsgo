#pragma once

namespace SDK
{
	class CUserCmd;
	class CBaseEntity;
}

class CFakewalk
{
public:
	int choked;
	void do_fakewalk(SDK::CUserCmd * cmd);
};

extern CFakewalk* slidebitch;