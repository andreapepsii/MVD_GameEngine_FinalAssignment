#pragma once
#include "includes.h"
#include "ScriptSystem.h"
#include "MovePlatformScript.h"

class SwitchScript : public Script {
public:
	SwitchScript(int owner) : Script(owner) {};
	Collider ownerCol;
	MovePlatformScript* mps;
	Transform *transform;

	void init();
	void update(float dt);
};
