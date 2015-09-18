#include <math.h>
#include <renderer.h>

using namespace glm;

class Util{

	float magnitude(vec3 v){
		return sqrtf(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
	}

};