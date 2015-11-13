#pragma once

#include "TornadoParticleEmitter.h"

class ParticleEmitterManager
{
private:

	map<string, ParticleEmitter*> emitters;
	effect particleShader;

public:
	ParticleEmitterManager();

	void update(const float delta_time, const map<string, SceneObject>& sceneObjects);
	void render(const mat4& PV);
	ParticleEmitter* getEmitter(const string &emitterName){ return emitters[emitterName]; }
	void add(const string &emitterName, ParticleEmitter* emitter){ emitters[emitterName] = emitter; }
	void remove(const string &emitterName){ emitters.erase(emitterName); }

	void compute();

	~ParticleEmitterManager();
};

