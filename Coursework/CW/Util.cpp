/*

Grant Smith (40111906)

TODO

*/

#include <math.h>
#include <renderer.h>
#include "Util.h"

using namespace glm;

using namespace Util;

	float Util::magnitude(vec3 v){
		return sqrtf(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
	}