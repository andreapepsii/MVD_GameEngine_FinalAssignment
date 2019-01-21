//
//  Game.cpp
//
//  Copyright � 2018 Alun Evans. All rights reserved.
//

#include "Game.h"
#include "Shader.h"
#include "extern.h"
#include "Parsers.h"
#include "FloorScript.h"
#include "SpawnControllerScript.h"
#include "NavmeshScript.h"
#include "MoveScript.h"


Game::Game() {

}

//helper functions to create stuff
int createObj(std::string name,
                 const lm::vec3& translate,
                 float rotateDegree,
                 const lm::vec3& rotateAxis,
                 const lm::vec3& scale,
                 int geom_id,
                 int mat_id,
				 const lm::vec3& local_center,
				 const lm::vec3& halfwidth) {
    int ent_new = ECS.createEntity(name);
    Mesh& fmc = ECS.createComponentForEntity<Mesh>(ent_new);
    fmc.geometry = geom_id; fmc.material = mat_id;
    ECS.getComponentFromEntity<Transform>(ent_new).rotateLocal(rotateDegree*DEG2RAD, rotateAxis); //rotate it flat
    ECS.getComponentFromEntity<Transform>(ent_new).scaleLocal(scale.x, scale.y, scale.z); //make it bigger
    ECS.getComponentFromEntity<Transform>(ent_new).translate(translate.x, translate.y, translate.z);
    //creates a collider with local size from -0.5 -> +0.5 in each axis around plane
    Collider& box_collider = ECS.createComponentForEntity<Collider>(ent_new);
    box_collider.collider_type = ColliderTypeBox;
    box_collider.local_center = local_center;
    box_collider.local_halfwidth = halfwidth;
    return ent_new;
}


int createCamera(std::string name,
                 const lm::vec3& position,
                 const lm::vec3& forward,
                 float fov,
                 float aspect,
                 float near,
                 float far) {
    int ent_cam = ECS.createEntity(name);
    Camera& cam = ECS.createComponentForEntity<Camera>(ent_cam);
    ECS.getComponentFromEntity<Transform>(ent_cam).translate(position.x, position.y, position.z);
    cam.position = lm::vec3(position.x, position.y, position.z);
    cam.forward = lm::vec3(forward.x, forward.y, forward.z);
    cam.setPerspective(fov*DEG2RAD, aspect, near, far);
    return ent_cam;
}

int createLight(std::string name,
                const lm::vec3& position,
                const lm::vec3& color) {
    int ent_light = ECS.createEntity(name);
    ECS.createComponentForEntity<Light>(ent_light);
    ECS.getComponentFromEntity<Transform>(ent_light).translate(position.x, position.y, position.z);
    ECS.getComponentFromEntity<Light>(ent_light).color = lm::vec3(color.x, color.y, color.z);
    return ent_light;
}

int createFree(const lm::vec3& position) {
	int ent_player = ECS.createEntity("Player");
	Camera& player_cam = ECS.createComponentForEntity<Camera>(ent_player);
	ECS.getComponentFromEntity<Transform>(ent_player).translate(position.x, position.y, position.z);
	player_cam.position = lm::vec3(0.0f, 3.0f, 5.0f);
	player_cam.forward = lm::vec3(0.0f, 0.0f, -1.0f);
	player_cam.setPerspective(60.0f*DEG2RAD, 1, 0.1f, 10000.0f);
	return ent_player;
}

int createPlayer(const lm::vec3& position, ControlSystem& sys) {
    int ent_player = ECS.createEntity("Player");
    Camera& player_cam = ECS.createComponentForEntity<Camera>(ent_player);
    ECS.getComponentFromEntity<Transform>(ent_player).translate(position.x, position.y, position.z);
    player_cam.position = lm::vec3(position.x, position.y, position.z);
    player_cam.forward = lm::vec3(0.0f, 0.0f, 0.85f);
    player_cam.setPerspective(60.0f*DEG2RAD, 1, 0.01f, 100.0f);

    //FPS colliders
    int ent_down_ray = ECS.createEntity("Down Ray");
    Transform& down_ray_trans = ECS.createComponentForEntity<Transform>(ent_down_ray);
    down_ray_trans.parent = ECS.getComponentID<Transform>(ent_player); //set parent as player entity *transform*!
    Collider& down_ray_collider = ECS.createComponentForEntity<Collider>(ent_down_ray);
    down_ray_collider.collider_type = ColliderTypeRay;
    down_ray_collider.direction = lm::vec3(0.0,-1.0,0.0);
    down_ray_collider.max_distance = 100.0f;
    
    int ent_left_ray = ECS.createEntity("Left Ray");
    Transform& left_ray_trans = ECS.createComponentForEntity<Transform>(ent_left_ray);
    left_ray_trans.parent = ECS.getComponentID<Transform>(ent_player); //set parent as player entity *transform*!
    Collider& left_ray_collider = ECS.createComponentForEntity<Collider>(ent_left_ray);
    left_ray_collider.collider_type = ColliderTypeRay;
    left_ray_collider.direction = lm::vec3(-1.0,0.0,0.0);
    left_ray_collider.max_distance = 2.0f;
    
    int ent_right_ray = ECS.createEntity("Right Ray");
    Transform& right_ray_trans = ECS.createComponentForEntity<Transform>(ent_right_ray);
    right_ray_trans.parent = ECS.getComponentID<Transform>(ent_player); //set parent as player entity *transform*!
    Collider& right_ray_collider = ECS.createComponentForEntity<Collider>(ent_right_ray);
    right_ray_collider.collider_type = ColliderTypeRay;
    right_ray_collider.direction = lm::vec3(1.0,0.0,0.0);
    right_ray_collider.max_distance = 2.0f;
    
    int ent_forward_ray = ECS.createEntity("Forward Ray");
    Transform& forward_ray_trans = ECS.createComponentForEntity<Transform>(ent_forward_ray);
    forward_ray_trans.parent = ECS.getComponentID<Transform>(ent_player); //set parent as player entity *transform*!
    Collider& forward_ray_collider = ECS.createComponentForEntity<Collider>(ent_forward_ray);
    forward_ray_collider.collider_type = ColliderTypeRay;
    forward_ray_collider.direction = lm::vec3(0.0,0.0,-1.0);
    forward_ray_collider.max_distance = 2.0f;
    
    int ent_back_ray = ECS.createEntity("Back Ray");
    Transform& back_ray_trans = ECS.createComponentForEntity<Transform>(ent_back_ray);
    back_ray_trans.parent = ECS.getComponentID<Transform>(ent_player); //set parent as player entity *transform*!
    Collider& back_ray_collider = ECS.createComponentForEntity<Collider>(ent_back_ray);
    back_ray_collider.collider_type = ColliderTypeRay;
    back_ray_collider.direction = lm::vec3(0.0,0.0,1.0);
    back_ray_collider.max_distance = 2.0f;
    
    //set all colliders for FPS control
	sys.FPS_collider_down = ECS.getComponentID<Collider>(ent_down_ray);
	sys.FPS_collider_left = ECS.getComponentID<Collider>(ent_left_ray);
	sys.FPS_collider_right = ECS.getComponentID<Collider>(ent_right_ray);
	sys.FPS_collider_forward = ECS.getComponentID<Collider>(ent_forward_ray);
	sys.FPS_collider_back = ECS.getComponentID<Collider>(ent_back_ray);
    
    return ent_player;
}

//Nothing here yet
void Game::init() {

	auto& pepe = ECS.getAllComponents<Collider>();

	//******* INIT SYSTEMS *******//

	//init systems except debug and scripts, which needs info about scene
	control_system_.init();
	graphics_system_.init();
	scripts_system_.init(&control_system_),
	

    //******** CREATE AND ADD RESOURCES **********//
    
    //shaders
	graphics_system_.loadShader("phong", "data/shaders/phong.vert", "data/shaders/phong.frag");
    
    //geometries
    int floor_geom_id = graphics_system_.createGeometryFromFile("data/assets/floor_5x5.obj");
    int plane_geom_id = graphics_system_.createPlaneGeometry();
	int teapot_geom_id = graphics_system_.createGeometryFromFile("data/assets/teapot_small.obj");
    
	//materials and textures
	int red_mat_id = graphics_system_.createMaterial();
	graphics_system_.getMaterial(red_mat_id).diffuse_map = Parsers::parseTexture("data/assets/red.tga");;
	graphics_system_.getMaterial(red_mat_id).shader_id = graphics_system_.getShaderProgram("phong");
	graphics_system_.getMaterial(red_mat_id).specular = lm::vec3(0, 0, 0);

	std::cout << "To get a red tile (End point) press: " << red_mat_id + 1 << endl;

	int green_mat_id = graphics_system_.createMaterial();
	graphics_system_.getMaterial(green_mat_id).diffuse_map = Parsers::parseTexture("data/assets/green.tga");;
	graphics_system_.getMaterial(green_mat_id).shader_id = graphics_system_.getShaderProgram("phong");
	graphics_system_.getMaterial(green_mat_id).specular = lm::vec3(0, 0, 0);

	std::cout << "To get a green tile (Start point) press: " << green_mat_id + 1 << endl;

	int white_mat_id = graphics_system_.createMaterial();
	graphics_system_.getMaterial(white_mat_id).diffuse_map = Parsers::parseTexture("data/assets/white.tga");;
	graphics_system_.getMaterial(white_mat_id).shader_id = graphics_system_.getShaderProgram("phong");
	graphics_system_.getMaterial(white_mat_id).specular = lm::vec3(0, 0, 0);

	std::cout << "To get a white tile (Floor) press: " << white_mat_id + 1 << endl;

	int purple_mat_id = graphics_system_.createMaterial();
	graphics_system_.getMaterial(purple_mat_id).diffuse_map = Parsers::parseTexture("data/assets/block_purple.tga");;
	graphics_system_.getMaterial(purple_mat_id).shader_id = graphics_system_.getShaderProgram("phong");
	graphics_system_.getMaterial(purple_mat_id).specular = lm::vec3(0, 0, 0);

	std::cout << "To get a pruple tile (Non-walkable floor) press: " << purple_mat_id +1 << endl;

	int black_mat_id = graphics_system_.createMaterial();
	graphics_system_.getMaterial(black_mat_id).diffuse_map = Parsers::parseTexture("data/assets/block_teal.tga");;
	graphics_system_.getMaterial(black_mat_id).shader_id = graphics_system_.getShaderProgram("phong");
	graphics_system_.getMaterial(black_mat_id).specular = lm::vec3(0, 0, 0);

	std::cout << "When the tiles are black, you can edit them with the previous mentioned buttons" << endl;

    //******* CREATE ENTITIES AND ADD COMPONENTS *******//

	//floor where the player stands
	int floor_player = createObj(
		"floor_stop", //name
		lm::vec3(1.25f,10,-20), //position
		0.0f, //rotation degree
		lm::vec3(1, 0, 0), //rotation axis
		lm::vec3(0.5, 0.5, 0.5), // scale
		floor_geom_id, //geometry id
		white_mat_id, //material id
		lm::vec3(-2.5, 0.125f, -2.5), //local_center of collider
		lm::vec3(2.5, 0.125, 2.5) //halfwidth of box collider
	);

	//generates floor tiles 
	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 8; j++) {
			int tag = (i * 8) + j;
			int floor = createObj("floor_" + to_string(tag), lm::vec3(15.0 - i * 2.5, 0.0, 10 - j * 2.5), 0.0, lm::vec3(1,0,0), lm::vec3(0.5,0.5,0.5),
				floor_geom_id, black_mat_id, lm::vec3(-2.5, 0.125f, -2.5), lm::vec3(2.5, 0.125, 2.5));
		}
	}
	
	//generates character to go throw grid
	int teapot = createObj("teapot", lm::vec3(0.0f, 13.0f, -21.5f), 0.0, lm::vec3(1.0, 0.0, 0.0), lm::vec3(1.0, 1.0, 1.0), teapot_geom_id, purple_mat_id, lm::vec3(0.0f, 0.5f, 0.0f), lm::vec3(0.75f, 0.5f, 0.5f));
	//players, cameras and lights
	int ent_light_1 = createLight("Light 1", lm::vec3(100.0f, 100.0f, 100.0f), lm::vec3(1.0f, 1.0f, 1.0f));
	int ent_player = createPlayer(lm::vec3(0.0f, 13.0f, -21.0f), control_system_);
    ECS.main_camera = ECS.getComponentID<Camera>(ent_player);

	//******* INIT SCRIPTS SYSTEM ******//

	
	//script to control floor set up and draw it 
	FloorScript* fs = new FloorScript(0);
	scripts_system_.registerScript(fs);
	//script to control character
	
	//script to control the player spawning with a minimum correct path
	SpawnControllerScript* scs = new SpawnControllerScript(teapot);
	scripts_system_.registerScript(scs);
	scs->fs = fs;
	
	
	//script to check where the character can move
	NavmeshScript* nms = new NavmeshScript(teapot);
	scripts_system_.registerScript(nms);
	nms->fs = fs;
	nms->scs = scs;
	
	//script to move character into next position
	MoveScript* ms = new MoveScript(teapot);
	scripts_system_.registerScript(ms);
	ms->nms = nms;
	
	//call init of all scripts registered with system
	scripts_system_.lateInit();
	

    //******* INIT DEBUG SYSTEM *******//
    debug_system_.init();
    debug_system_.setActive(true);
}

//Entry point for game update code
void Game::update(float dt) {
	//update each system in turn

	//update input
	control_system_.update(dt);

	//collision
	collision_system_.update(dt);

	//update scripts
	scripts_system_.update(dt);

	//render
	graphics_system_.update(dt);
    
    //debug
    debug_system_.update(dt);
    
}

