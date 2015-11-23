/*
Grant Smith (40111906)

Manages particle emitters that manage the emitting of particles

*/

#pragma once

#include "TornadoParticleEmitter.h"

class ParticleEmitterManager
{
private:

	map<string, ParticleEmitter*> emitters; // List of all of the emitters
	map<string, texture> textures; // List of the textures used by the emitters
	effect particleShader; // The shader used by the emitters

public:
	// Constructor for the manager
	ParticleEmitterManager();

	// Update the emitters
	void update(const float delta_time);
	// render the emitters
	void render(const mat4& PV);
	// get an emitter by name
	ParticleEmitter* getEmitter(const string &emitterName){ return emitters[emitterName]; }
	// add an emitter with a name
	void add(const string &emitterName, ParticleEmitter* emitter, const string texturePath);
	// remove an emitter by name 
	void remove(const string &emitterName){ emitters.erase(emitterName); }

	// get the number of particles within this manager
	int getParticleCount();
	
	// Delete the manager
	~ParticleEmitterManager(){}
};

