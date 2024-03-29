//
//  Game.h
//  02-GameLoop
//
//  Copyright � 2018 Alun Evans. All rights reserved.
//
#pragma once
#include "includes.h"
#include "GraphicsSystem.h"
#include "ControlSystem.h"
#include "DebugSystem.h"
#include "CollisionSystem.h"
#include "ScriptSystem.h"


class Game
{
public:
	Game();

	static Game* instance;

	void init();
	void update(float dt);

	//pass input straight to input system
	void updateMousePosition(int new_x, int new_y) { 
		control_system_.updateMousePosition(new_x, new_y);
	}
	void key_callback(int key, int scancode, int action, int mods) {
		control_system_.key_mouse_callback(key, action, mods);
	}
	void mouse_button_callback(int button, int action, int mods) {
		control_system_.key_mouse_callback(button, action, mods);
	}

private:
	GraphicsSystem graphics_system_;
	ControlSystem control_system_;
    DebugSystem debug_system_;
    CollisionSystem collision_system_;
	ScriptSystem scripts_system_;
};
