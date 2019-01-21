#pragma once
#include "includes.h"
#include "ScriptSystem.h"
#include "extern.h"


class FloorScript : public Script {
public:
	FloorScript(int owner) : Script(owner) {};
	Transform *transform;

	void init();
	void update(float dt);

	const int rowSize = 8;
	const int colSize = 12;
	Mat floor_matrix;
	int currentRow;
	int currentCol;

	int mat_value;

	bool restart;
	bool placedAllTiles;
	bool currentPressed;
	bool previousPressed;
};