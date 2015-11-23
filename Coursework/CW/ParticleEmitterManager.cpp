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

// Update the emitters
void ParticleEmitterManager::update(const float delta_time){
	typedef map<string, ParticleEmitter*>::iterator it_type;
	for (it_type iterator = emitters.begin(); iterator != emitters.end(); ++iterator) {
		iterator->second->update(delta_time);
	}
}

// add an emitter with a name
void ParticleEmitterManager::add(const string &emitterName, ParticleEmitter* emitter, const string texturePath){
	if (textures.find(texturePath) == textures.end())
		textures[texturePath] = Util::loadTexture(texturePath);

	emitter->tex = &textures[texturePath];
	emitter->particleShader = &particleShader;
	emitters[emitterName] = emitter;
}

// render the emitters
void ParticleEmitterManager::render(const mat4& PV){
	typedef map<string, ParticleEmitter*>::iterator it_type;
	for (it_type iterator = emitters.begin(); iterator != emitters.end(); ++iterator) {
		iterator->second->render(PV);
	}
}

// get the number of particles within this manager
int ParticleEmitterManager::getParticleCount(){
	int sum = 0;
	for (pair<string, ParticleEmitter*> e : emitters){
		sum += e.second->particleCount;
	}
	return sum;
}
