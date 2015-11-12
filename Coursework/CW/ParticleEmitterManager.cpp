#include "ParticleEmitterManager.h"


ParticleEmitterManager::ParticleEmitterManager()
{
}

void ParticleEmitterManager::update(const float delta_time, const map<string, SceneObject>& sceneObjects){
	for (pair<string, ParticleEmitter*> e : emitters){
		e.second->update(delta_time, sceneObjects);
	}
}

void ParticleEmitterManager::render(const mat4& PV){
	for (pair<string, ParticleEmitter*> e : emitters){
		e.second->render(PV);
	}
}

ParticleEmitterManager::~ParticleEmitterManager()
{
}
