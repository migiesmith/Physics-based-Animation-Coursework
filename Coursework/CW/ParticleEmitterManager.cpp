#include "ParticleEmitterManager.h"


ParticleEmitterManager::ParticleEmitterManager()
{
	//particleShader.add_shader("..\\resources\\shaders\\particle_shader.comp", GL_COMPUTE_SHADER);

	// Build effect
	//particleShader.build();
}

void ParticleEmitterManager::update(const float delta_time){
	for (pair<string, ParticleEmitter*> e : emitters){
		e.second->update(delta_time);
	}
}


int ParticleEmitterManager::getParticleCount(){
	int sum = 0;
	for (pair<string, ParticleEmitter*> e : emitters){
		sum += e.second->particleCount;
	}
	return sum;
}

void ParticleEmitterManager::render(const mat4& PV){
	for (pair<string, ParticleEmitter*> e : emitters){
		e.second->render(PV);
	}
}

ParticleEmitterManager::~ParticleEmitterManager()
{
}
