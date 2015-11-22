// Spatial Partitioning Grid

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

	SPGrid(){};

	// Dimensions of the grid:
	//  x	   y	   z
	int _WIDTH, _HEIGHT, _DEPTH;
	int _cellSize;
	vec3 _offset;

	// The cells within the grid
	SPCell* _cells;

	int cellIndex(const int& x, const int& y, const int& z);
	int incremenetedCellIndex(const int& currentIndex, const int& x, const int& y, const int& z);

public:

	static SPGrid& SPGrid::getInstance(){
		static SPGrid _instance;

		return _instance;
	}

	void SPGrid::init(const int& WIDTH, const int& HEIGHT, const int& DEPTH, const int& cellSize);
	void init(const int& WIDTH, const int& HEIGHT, const int& DEPTH, const int& cellSize, const vec3& offset);
	void setBasePos(const int& x, const int& y, const int& z);

	void intersects(Collider& inCollider, const vec3& velocity, IntersectionData& data);

	void update(const map<string, SceneObject>& sceneObjects);

	vec3 getXYZInGrid(const vec3& v);
	int getPosInGrid(const vec3& v);

	
	void render(effect& shader, bool renderFullGrid);


	SPGrid(SPGrid const&) = delete;
	void operator=(SPGrid const&) = delete;
};

