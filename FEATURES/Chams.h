#pragma once

class CChams
{
public:
	SDK::IMaterial* CreateMaterialMetallic(bool ignorez, bool lit, bool wireframe);
	SDK::IMaterial* CreateMaterialBasic(bool ignorez, bool lit, bool wireframe);
private:
	void InitKeyValues(SDK::KeyValues* keyValues, std::string name);
	void LoadFromBuffer(SDK::KeyValues* keyValues, std::string name_, std::string buffer_);
};

extern CChams* chams;