#pragma once

#include "includes.h"
#include "SDK/IMaterial.h"

class MaterialBackup {

public:
	SDK::MaterialHandle_t handle;
	SDK::IMaterial* material;
	float color[3];
	float alpha;
	bool translucent;
	bool nodraw;

	MaterialBackup() {

	}

	MaterialBackup(SDK::MaterialHandle_t h, SDK::IMaterial* p) {
		handle = handle;
		material = p;

		material->GetColorModulation(&color[0], &color[1], &color[2]);
		alpha = material->GetAlphaModulation();
	}

	void restore() {
		material->ColorModulate(color[0], color[1], color[2]);
		material->AlphaModulate(alpha);
		material->SetMaterialVarFlag(SDK::MATERIAL_VAR_TRANSLUCENT, translucent);
		material->SetMaterialVarFlag(SDK::MATERIAL_VAR_NO_DRAW, nodraw);
	}
};

class nightmode {
public:
	void clear_stored_materials();
	void modulate(SDK::MaterialHandle_t i, SDK::IMaterial *material, bool backup);
	void apply();
	void remove();
};