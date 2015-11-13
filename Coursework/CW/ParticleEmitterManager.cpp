#include "ParticleEmitterManager.h"


ParticleEmitterManager::ParticleEmitterManager()
{
	particleShader.add_shader("..\\resources\\shaders\\particle_shader.comp", GL_COMPUTE_SHADER);

	// Build effect
	particleShader.build();



	/*
	renderer::bind(particleShader);

	vector<vec4> shader_data = {
		vec4(0, 0, 0, 1)
	};
	GLuint ssbo = 0;
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(shader_data), &shader_data, GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	memcpy(p, &shader_data, sizeof(shader_data));
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	GLuint block_index = 0;
	block_index = glGetProgramResourceIndex(particleShader.get_program(), GL_SHADER_STORAGE_BLOCK, "shader_data");

	/*
	glShaderStorageBlockBinding(particleShader.get_program(), block_index, ssbo_binding_point_index);

	glShaderStorageBlockBinding(particleShader.get_program(), block_index, 80);

	GLuint binding_point_index = 80;
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point_index, ssbo);
	glDispatchCompute(4,4,4);
	*/
}

void ParticleEmitterManager::update(const float delta_time, const map<string, SceneObject>& sceneObjects){
	for (pair<string, ParticleEmitter*> e : emitters){
		e.second->update(delta_time, sceneObjects);
	}

}


void ParticleEmitterManager::compute(){

}

void ParticleEmitterManager::render(const mat4& PV){
	for (pair<string, ParticleEmitter*> e : emitters){
		e.second->render(PV);
	}
}

ParticleEmitterManager::~ParticleEmitterManager()
{
}
