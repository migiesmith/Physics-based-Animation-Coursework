#include "IKManager.h"

IKManager::IKManager()
{
}


void IKManager::update(){
	for (auto& ikHierarchy : _ikHierarchies){
		ikHierarchy.second.update();
	}
}

void IKManager::render(mat4& VP, effect* inShader){
	for (auto& ikHierarchy : _ikHierarchies){
		ikHierarchy.second.render(VP, *inShader);
	}
}

IKManager::~IKManager()
{
}
