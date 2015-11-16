#include "ParticleEmitter.h"


ParticleEmitter::ParticleEmitter(const vec3& v, const int particleCount, const vec3& force, const float lifeTime, const string texturePath, const int columns, const int rows) : vec3(v)
{
	this->force = force;
	this->lifeTime = lifeTime;
	this->particleCount = particleCount;
	for (int i = 0; i < particleCount; i++){
		Particle& p = Particle(v, 1.0f);
		p.isAlive = false;
		particles.push_back(p);
	}
	// Add Shaders
	particleEffect.add_shader("..\\resources\\shaders\\passthrough_shaders\\texture_passthrough.vert", GL_VERTEX_SHADER);
	particleEffect.add_shader("..\\resources\\shaders\\particle_shader\\particle_shader.geo", GL_GEOMETRY_SHADER);
	particleEffect.add_shader("..\\resources\\shaders\\particle_shader\\particle_shader.frag", GL_FRAGMENT_SHADER);

	// Build effect
	particleEffect.build();

	tex = Util::loadTexture(texturePath);
	this->columns = columns;
	this->rows = rows;

}

void ParticleEmitter::update(const float delta_time){
	IntersectionData& data = IntersectionData();
	SPGrid& spGrid = SPGrid::getInstance();

	for (Particle& p : particles){
		if (p.isAlive){
			data.reset();
			spGrid.intersects(*p.collider, p.velocity, data);
			
			if (data.doesIntersect){
				p.addForce(data.direction * data.amount);
				p.velocity -= data.direction*dot(data.direction, p.velocity);
			}
			p.update(delta_time);
		}else if(emitTimer <= 0.0f){
			awakenParticle(p);
		}
	}

	emitTimer -= delta_time;
}

void ParticleEmitter::awakenParticle(Particle& p){
	vec3 offset = normalize(force);

	for (int i = 0; i < 3; i++){
		offset[i] = ((((rand() % 200) - 100.0f) / 100.0f) * (1.0f - offset[i])) * 120.0f;
	}

	p.awaken(*this, force + offset, lifeTime);
	emitTimer = 1.0f / emitSpeed;
}

void ParticleEmitter::render(const mat4& PV){

	glAlphaFunc(GL_GREATER, 0.2f);
	glEnable(GL_ALPHA_TEST);
	renderer::bind(particleEffect);

	glUniformMatrix4fv(
		particleEffect.get_uniform_location("MVP"), // Location of uniform
		1, // Number of values - 1 mat4
		GL_FALSE, // Transpose the matrix?
		value_ptr(PV)); // Pointer to matrix data

	renderer::bind(tex, 0);

	glUniform4fv(particleEffect.get_uniform_location("colour"), 1, value_ptr(colour));
	glUniform1f(particleEffect.get_uniform_location("lifeTime"), lifeTime);
	glUniform1f(particleEffect.get_uniform_location("xCoordInterval"), 1.0f / columns);
	glUniform1f(particleEffect.get_uniform_location("yCoordInterval"), 1.0f / rows);
	for (Particle& p : particles){
		glUniform1f(particleEffect.get_uniform_location("totalTime"), lifeTime - p.lifeTime);
		p.render();
	}
	glDisable(GL_ALPHA_TEST);
}

void ParticleEmitter::setPosition(const vec3& v){
	x = v.x;
	y = v.y;
	z = v.z;
}

void ParticleEmitter::setColour(const vec4& v){
	this->colour = v;
}

ParticleEmitter::~ParticleEmitter()
{

}
