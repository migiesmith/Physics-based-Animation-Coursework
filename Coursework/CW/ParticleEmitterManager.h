#pragma once

#include "TornadoParticleEmitter.h"

class ParticleEmitterManager
{
private:

	map<string, ParticleEmitter*> emitters;
	map<string, texture> textures;
	effect particleShader;

public:
	ParticleEmitterManager();

	void update(const float delta_time);
	void render(const mat4& PV);
	ParticleEmitter* getEmitter(const string &emitterName){ return emitters[emitterName]; }
	void add(const string &emitterName, ParticleEmitter* emitter, const string texturePath);
	void remove(const string &emitterName){ emitters.erase(emitterName); }

	int getParticleCount();
	
	~ParticleEmitterManager();
};

