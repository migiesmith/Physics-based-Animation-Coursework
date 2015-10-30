#include "ParticleEmitterManager.h"


ParticleEmitterManager::ParticleEmitterManager()
{
}

void ParticleEmitterManager::update(const float delta_time){
	for (pair<string, ParticleEmitter*> e : emitters){
		e.second->update(delta_time);
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
