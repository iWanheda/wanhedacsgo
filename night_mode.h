#pragma once

#include "includes.h"
#include "UTILS/interfaces.h"
#include "SDK/CClientEntityList.h"
#include "SDK/IEngine.h"
#include "SDK/ConVar.h"

class cnight_mode {

public:
	void do_nightmode();
}; extern cnight_mode* nightmode;