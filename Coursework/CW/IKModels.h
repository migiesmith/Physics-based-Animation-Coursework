/*
Grant Smith (40111906)

Manages the models used by the ik hierarchy (makes sure that multiple of the same model aren't loaded)

*/

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

	// Hide the default constructor (Singleton)
	IKModels(){};

	// Stores the list of loaded models
	map<string, geometry> models;

public:

	// Overrides the array operator to access the models
	geometry &operator[](const string& id)
	{
		return models[id];
	}

	// Loads a model and adds it to the list of models
	void addModel(const string &id){
		if (models.find(id) == models.end())
			models[id] = Util::loadModel("ikmodels\\" + id);
	}

	// Return the only instance of IKModels (Singleton)
	static IKModels& IKModels::getInstance(){
		static IKModels _instance;

		return _instance;
	}

	// Remove the destructors (Singleton)
	IKModels(IKModels const&) = delete;
	void operator=(IKModels const&) = delete;
};

