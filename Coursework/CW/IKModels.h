#pragma once

#include <renderer.h>
#include "Util.h"
#include "IKHierarchy.h"

using namespace std;
using namespace graphics_framework;
using namespace glm;

class IKModels
{
private:

	IKModels(){};

	map<string, geometry> models;
	texture _texture;

public:
	geometry &operator[](const string& id)
	{
		return models[id];
	}

	void addModel(const string &id){
		if (models.find(id) == models.end())
			models[id] = Util::loadModel("ikmodels\\" + id);
	}

	static IKModels& IKModels::getInstance(){
		static IKModels _instance;

		return _instance;
	}

	IKModels(IKModels const&) = delete;
	void operator=(IKModels const&) = delete;
};

