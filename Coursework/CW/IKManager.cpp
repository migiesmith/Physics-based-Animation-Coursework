#include "IKManager.h"

// Create the Manager
IKManager::IKManager()
{
}

// Update all hierarchies
void IKManager::update(){
	for (auto& ikHierarchy : _ikHierarchies){
		ikHierarchy.second.update();
	}
}

// Render all hierarchies
void IKManager::render(mat4& VP, effect* inShader){
	for (auto& ikHierarchy : _ikHierarchies){
		ikHierarchy.second.render(VP, *inShader);
	}
}

// Delete the Manager
IKManager::~IKManager()
{
}
