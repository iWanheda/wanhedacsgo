#pragma once

class CEnginePred
{
public:
	void run_prediction(SDK::CUserCmd * cmd);
	void end_prediction(SDK::CUserCmd * cmd);
};

extern CEnginePred* prediction;