#include "SwitchScript.h"
#include "extern.h"

void SwitchScript::init() {
	//get transform of entity
	transform = &ECS.getComponentFromEntity<Transform>(owner_);
}


void SwitchScript::update(float dt) {

	ownerCol = ECS.getComponentFromEntity<Collider>(owner_);
	if (ownerCol.colliding) {
		mps->shouldPlatformMove = true;
	}
	else {
		mps->shouldPlatformMove = false;
	}

	//std::cout << ownerCol.colliding << std::endl;

}