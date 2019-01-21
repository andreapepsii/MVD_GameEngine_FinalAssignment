#include "MovePlatformScript.h"
#include "extern.h"

void MovePlatformScript::init() {
	//get transform of entity
	transform = &ECS.getComponentFromEntity<Transform>(owner_);
}


void MovePlatformScript::update(float dt) {

	if (input_->GetKey(GLFW_KEY_J)) {
		shouldPlatformMove = true;
	}

	if(input_->GetKey(GLFW_KEY_K)) {
		shouldPlatformMove = false;
	}

	if (shouldPlatformMove) {
		transform->translate(0, 1*dt, 0);
	}

	//std::cout << shouldPlatformMove << std::endl;

}