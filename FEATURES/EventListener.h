#pragma once

#include "../SDK/GameEvents.h"

struct ImpactInfo {
	float x, y, z;
	long long time;
};

struct HitmarkerInfo {
	ImpactInfo impact;
	float alpha;
};
class HitmarkerStuff
{
public:
	std::vector<ImpactInfo> impacts;
	std::vector<HitmarkerInfo> hitmarkers;
};
extern HitmarkerStuff* hitmarkerstuff;
