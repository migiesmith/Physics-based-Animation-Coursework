#include "ParticleEmitterManager.h"


ParticleEmitterManager::ParticleEmitterManager()
{
	// Add Shaders
	particleShader.add_shader("..\\resources\\shaders\\passthrough_shaders\\texture_passthrough.vert", GL_VERTEX_SHADER);
	particleShader.add_shader("..\\resources\\shaders\\particle_shader\\particle_shader.geo", GL_GEOMETRY_SHADER);
	particleShader.add_shader("..\\resources\\shaders\\particle_shader\\particle_shader.frag", GL_FRAGMENT_SHADER);

	// Build effect
	particleShader.build();
}

void ParticleEmitterManager::update(const float delta_time){
	for (pair<string, ParticleEmitter*> e : emitters){
		e.second->update(delta_time);
	}
}

void ParticleEmitterManager::add(const string &emitterName, ParticleEmitter* emitter, const string texturePath){
	if (textures.find(texturePath) == textures.end())
		textures[texturePath] = Util::loadTexture(texturePath);

	emitter->tex = &textures[texturePath];
	emitter->particleShader = &particleShader;
	emitters[emitterName] = emitter;
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
