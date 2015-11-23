/*

Grant Smith (40111906) - 23/11/2015

The SPGrid that handles the SPCells which handle collision detection and resolution

*/
#pragma once

#include "SPCell.h"
#include "SceneObject.h"
#include "CubeCollider.h"
#include <renderer.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

class SPGrid
{
private:

	// Hide the default constructor (Singleton)
	SPGrid(){};

	// Dimensions of the grid:
	//  x	   y	   z
	int _WIDTH, _HEIGHT, _DEPTH;
	// Size of each cell (xyz are the same)
	int _cellSize;
	// Offset from the origin
	vec3 _offset;

	// The cells within the grid
	SPCell* _cells;

	// Returns the 1D array location of the cell from a 3D array position
	int cellIndex(const int& x, const int& y, const int& z);

public:

	// Get the only instance of the grid (Singleton)
	static SPGrid& SPGrid::getInstance(){
		static SPGrid _instance;

		return _instance;
	}

	// Initialises the grid
	void SPGrid::init(const int& WIDTH, const int& HEIGHT, const int& DEPTH, const int& cellSize);
	void init(const int& WIDTH, const int& HEIGHT, const int& DEPTH, const int& cellSize, const vec3& offset);

	// Set the where the bottom, center of the grid is
	void setBasePos(const int& x, const int& y, const int& z);

	// Handles collision for this collider
	void intersects(Collider& inCollider, const vec3& velocity, IntersectionData& data);

	// Updates the grid with scene objects
	void update(const map<string, SceneObject>& sceneObjects);

	// Returns the 1D array location of the cell from a world position
	int getPosInGrid(const vec3& v);
	// Returns the 3D array position of the grid from a world position
	vec3 getXYZInGrid(const vec3& v);


	// Render the grid, only for debugging
	void render(effect& shader, bool renderFullGrid);

	// Removs the deleteion functions (Singleton)
	SPGrid(SPGrid const&) = delete;
	void operator=(SPGrid const&) = delete;
};

