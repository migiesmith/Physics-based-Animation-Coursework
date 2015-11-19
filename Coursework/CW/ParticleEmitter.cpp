#include "ParticleEmitter.h"


ParticleEmitter::ParticleEmitter(const vec3& v, const int particleCount, const vec3& force, const float lifeTime, const string texturePath, const int columns, const int rows) : vec3(v)
{
	this->force = force;
	this->lifeTime = lifeTime;
	this->particleCount = particleCount;
	for (int i = 0; i < particleCount; i++){
		Particle& p = Particle(v, 0.1f);
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
			p.update(delta_time);
			spGrid.intersects(*p.collider, p.collider->velocity, data);
			
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


	struct Vertex {
		GLfloat position[3];
		GLfloat vertTime;
	};
	const int NUM_VERTS = particles.size();
	Vertex* vertexdata = new Vertex[NUM_VERTS];

	int vertNum = 0;
	for (Particle& p : particles){
		// Add particle to vertexdata
		vertexdata[vertNum] = { { p.x, p.y, p.z }, { lifeTime - p.lifeTime } };
		vertNum++;
	}


	// Create and bind a VAO
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and bind a BO for vertex data
	GLuint vbuffer;
	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);

	// copy data into the buffer object
	glBufferData(GL_ARRAY_BUFFER, NUM_VERTS * sizeof(Vertex), vertexdata, GL_STATIC_DRAW);

	delete[] vertexdata;

	int posAttrib = glGetAttribLocation(particleEffect.get_program(), "position");
	int totalTimeAttrib = glGetAttribLocation(particleEffect.get_program(), "totalTime");
	
	// set up vertex attributes
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(totalTimeAttrib);
	glVertexAttribPointer(totalTimeAttrib, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, vertTime));


	// Remder the string
	renderer::bind(tex, 0);
	glAlphaFunc(GL_GREATER, 0.2f);
	glEnable(GL_ALPHA_TEST);
	glDrawArrays(GL_POINTS, 0, NUM_VERTS);
	glDisable(GL_ALPHA_TEST);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(posAttrib);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffersARB(1, &vbuffer);

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
