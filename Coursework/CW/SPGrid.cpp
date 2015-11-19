#include "SPGrid.h"

void SPGrid::init(const int& WIDTH, const int& HEIGHT, const int& DEPTH, const int& cellSize){
	this->_WIDTH = WIDTH;
	this->_HEIGHT = HEIGHT;
	this->_DEPTH = DEPTH;
	this->_cellSize = cellSize;
	_cells = new SPCell[WIDTH * HEIGHT * DEPTH];
}

void SPGrid::init(const int& WIDTH, const int& HEIGHT, const int& DEPTH, const int& cellSize, const vec3& offset){
	init(WIDTH, HEIGHT, DEPTH, cellSize);

	this->_offset = offset - vec3((_WIDTH * _cellSize) * 0.5f, (_HEIGHT * _cellSize) * 0.5f, (_DEPTH * _cellSize) * 0.5f);
}


void SPGrid::setBasePos(const int& x, const int& y, const int& z){
	_offset = vec3(x,y,z) - vec3((_WIDTH * _cellSize) * 0.5f, 0, (_DEPTH * _cellSize) * 0.5f);
}

int SPGrid::getPosInGrid(const vec3& v){
	// Math.floor( (position - gridMinimum) / gridCellSize )
	vec3 pos = getXYZInGrid(v);
	//grid[col][cell] = ... // bucket to put entity into
	return cellIndex(pos.x, pos.y, pos.z);
}

vec3 SPGrid::getXYZInGrid(const vec3& v){
	int row = floor((v.x - _offset.x) / _cellSize);
	int col = floor((v.y - _offset.y) / _cellSize);
	int cell = floor((v.z - _offset.z) / _cellSize);
	return vec3(row, col, cell);
}

vec3 SPGrid::xyzCellPosition(const int& pos){
	int z = pos / (_WIDTH * _HEIGHT);
	int y = (pos - z * _WIDTH * _HEIGHT) / _WIDTH;
	int x = pos - _WIDTH * (y + _HEIGHT * z);
	return vec3(x, y, z);
}

void SPGrid::intersects(Collider& inCollider, const vec3& velocity, IntersectionData& data){
	int posInGrid = getPosInGrid(inCollider.position);
	if (posInGrid != -1)
		_cells[posInGrid].intersects(inCollider, velocity, data);
}


void SPGrid::update(const map<string, SceneObject>& sceneObjects){
	for (int i = 0; i < _WIDTH * _HEIGHT * _DEPTH; i++){
		_cells[i].clear();
	}
	for (auto& mapObj : sceneObjects){
		SceneObject& sO = (SceneObject&)mapObj.second;

		Collider* soCollider = sO.getCollider();
		if (soCollider){
			int posInGrid = getPosInGrid(sO.get_transform().position);

			// TODO fix the check to see if an object is within multiple partitions
			if (soCollider->getType() == ColliderTypes::OBBCUBE || soCollider->getType() == ColliderTypes::CUBE){
				CubeCollider* cubeCollider = (CubeCollider*)soCollider;
				
				/*
				vector<vec3>& corners = cubeCollider->getCorners();
				
				vec3 topRight = normalize(vec3(1, 1, 1));
				vec3 max;
				vec3 min;
				float maxVal = numeric_limits<float>().min();
				float minVal = numeric_limits<float>().max();
				for (vec3& corner : corners){
					_cells[getPosInGrid(corner)].addCollider(sO.getCollider());
				}
				*/

			}
			else if (soCollider->getType() == ColliderTypes::PLANE){
				for (int x = 0; x < _WIDTH; x++){
					for (int z = 0; z < _DEPTH; z++){
						_cells[cellIndex(x, 0, z)].addCollider(sO.getCollider());
					}
				}
			}

			if (posInGrid != -1){
				_cells[posInGrid].addCollider(sO.getCollider());
			}
		}
	}
}

// Render the grid, only for debugging
void SPGrid::render(){
	for (int x = 0; x < _WIDTH; x++){

		for (int y = 0; y < _HEIGHT; y++){

			for (int z = 0; z < _DEPTH; z++){
				if (_cells[cellIndex(x, y, z)].beenTested > 0){
					vec3 p = vec3(_offset.x + x*_cellSize, _offset.y + y*_cellSize, _offset.z + z*_cellSize);
					glBegin(GL_LINE_LOOP);
					glVertex3f(p.x, p.y, p.z);
					glVertex3f(p.x, p.y + _cellSize, p.z);
					glVertex3f(p.x + _cellSize, p.y + _cellSize, p.z);
					glVertex3f(p.x + _cellSize, p.y, p.z);

					glVertex3f(p.x, p.y, p.z);
					glVertex3f(p.x, p.y + _cellSize, p.z);
					glVertex3f(p.x, p.y + _cellSize, p.z + _cellSize);
					glVertex3f(p.x, p.y, p.z + _cellSize);

					glVertex3f(p.x + _cellSize, p.y, p.z + _cellSize);
					glVertex3f(p.x + _cellSize, p.y + _cellSize, p.z + _cellSize);
					glVertex3f(p.x + _cellSize, p.y + _cellSize, p.z);
					glVertex3f(p.x + _cellSize, p.y, p.z);

					glVertex3f(p.x + _cellSize, p.y, p.z + _cellSize);
					glVertex3f(p.x + _cellSize, p.y + _cellSize, p.z + _cellSize);
					glVertex3f(p.x, p.y + _cellSize, p.z + _cellSize);
					glVertex3f(p.x, p.y, p.z + _cellSize);

					glEnd();
				}
			}
		}
	}
}

int SPGrid::incremenetedCellIndex(const int& currentIndex, const int& x, const int& y, const int& z){
	int returnIndex = currentIndex;
	returnIndex += x + _WIDTH * (y + _HEIGHT * z);
	if (returnIndex < _WIDTH*_HEIGHT*_DEPTH && returnIndex >= 0)
		return returnIndex;
	else return -1;
}

int SPGrid::cellIndex(const int& x, const int& y, const int& z){
	if (x >= 0 && x < _WIDTH && y >= 0 && y < _HEIGHT && z >= 0 && z < _DEPTH)
		return x + _WIDTH * (y + _HEIGHT * z);
	else
		return -1;
}
