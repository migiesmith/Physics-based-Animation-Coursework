/*
	Grant Smith (40111906)

	The main class handles the loading in of models and textures, and the updating and rendering of the SceneObjects

*/


#define DBG_HALT __asm {int 3}
#if defined(DEBUG) | defined(_DEBUG)
#define DBG_ASSERT(exp) {if(!(exp)) {DBG_HALT;}}
#else
#define DBG_ASSERT(exp) {}
#endif

#include "main.h"


// Camera Variables
target_camera targetCam;
free_camera freeCam;
vec3 camPos; // The position of the current camera

map<string, mesh> meshes; // A map containing all of the meshes
map<string, SceneObject> sceneObjects; // A map containing all of the main sceneobjects
map<string, texture> texs; // A map containing all of the textures

mesh skyBox[3]; // The meshs for the skybox


bool isWireframe = false; // Whether or not to render in wireframe

vec3 ambientLightPosition = vec3(0, 400, 0); // Set the position of the ambient light

double cursor_x = 0.0; // Stores the position X of the cursor
double cursor_y = 0.0; // Stores the position Y of the cursor

float totalTime = 0.0f; // Stores how much time has passed (used for time based events like model bobbing and light movement)
float totalPhysicsTime = 0.0f; // Stores how much time has passed for the physics engine (used for time based events such as ik walking)

// The manager for the particle system
ParticleEmitterManager* particManager;

// The manager for the inverse kinematic system
IKManager ikManager;

void keyListener(GLFWwindow* window, int key, int scancode, int action, int mods){

	if (key == GLFW_KEY_F1 && action == GLFW_PRESS){
		isWireframe = !isWireframe;
	}
	else  if (key == GLFW_KEY_F2 && action == GLFW_PRESS){
		currentCamera = Camera::Target;
	}
	else if (key == GLFW_KEY_F3 && action == GLFW_PRESS){
		currentCamera = Camera::Free;
	}
	else if (key == GLFW_KEY_H && action == GLFW_PRESS){
		toggleDebugMenu = !toggleDebugMenu;
	}
}

void mouseListener(GLFWwindow* window, int button, int action, int mods){

	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS){

		if (toggleDebugMenu){
			// Get the current mouse pos
			double current_x;
			double current_y;
			glfwGetCursorPos(renderer::get_window(), &current_x, &current_y);

			double w = renderer::get_screen_width();
			double h = renderer::get_screen_height();

			// Convert mouse into screen space coords (0-1)
			current_x /= w;
			current_y /= h;
			double ratio = 1.0f / 7.0f;
			double buttonWidth = (ratio * 0.63f);
			double buttonHeight = (ratio * 0.5f);

			bool buttonClicked = false;

			// Loop through to check what button has been pressed
			for (int i = 1; i < 8; i++){
				for (int j = 0; j < 2; j++){
					if (current_x < buttonWidth*i && current_x > buttonWidth*(i - 1)
						&& current_y < 1.0 - buttonHeight*j && current_y > 1.0 - buttonHeight*(j + 1)){
						switch (i + (7 * j) - 1){
						case Buttons::Particles:
							toggleParticles = !toggleParticles;
							buttonClicked = true;
							break;
						case Buttons::IK:
							toggleIK = !toggleIK;
							buttonClicked = true;
							break;
						case Buttons::FullSPGrid:
							toggleFullSPGrid = !toggleFullSPGrid;
							buttonClicked = true;
							break;
						case Buttons::SPGrid:
							toggleSPGrid = !toggleSPGrid;
							buttonClicked = true;
							break;
						case Buttons::ResetParticles:
							initParticleManager();
							buttonClicked = true;
							break;
						}
					}
				}
			}

			if (!buttonClicked)
				attemptToMakeAParticleEmitter();
		}
		else{
			attemptToMakeAParticleEmitter();
		}

	}

}

// Sends a ray out, if it hits the plane, it creates an emitter at the intersection point
void attemptToMakeAParticleEmitter(){
	if (!toggleParticles) return;
	/*
	Gets the current mouse position
	*/
	double current_x;
	double current_y;
	glfwGetCursorPos(renderer::get_window(), &current_x, &current_y);

	// Initialize variables required to create ray
	float dx, dy;
	float aspect = static_cast<float>(renderer::get_screen_width()) / static_cast<float>(renderer::get_screen_height());
	mat4 invMatrix, viewMatrix;

	// Get the screen coords for projecting
	dx = tanf(quarter_pi<float>()*0.5f)*(((float)current_x / (float)renderer::get_screen_width() - 0.5f)*2.0f) * aspect;
	dy = tanf(quarter_pi<float>()*0.5f)*((1.0f - (float)current_y / (float)renderer::get_screen_height() - 0.5f)*2.0f);

	// Project the coords into world space
	viewMatrix = freeCam.get_view();
	invMatrix = inverse(viewMatrix);
	vec3 rayStart = vec3(dx*MYNEAR, dy*MYNEAR, -MYNEAR);
	vec3 rayEnd = vec3(dx*MYFAR, dy*MYFAR, -MYFAR);

	rayStart = vec4ToVec3(invMatrix * vec3ToVec4(rayStart));
	rayEnd = vec4ToVec3(invMatrix * vec3ToVec4(rayEnd));

	// Get the ray direction
	vec3 rayDir = normalize(rayEnd - rayStart);

	// Get the intersection point
	vec3 P = ((PlaneCollider*)sceneObjects["ground"].getCollider())->rayIntersection(rayStart, rayDir);

	// Check if the ray is within scene bounds
	if (SPGrid::getInstance().getPosInGrid(P) != -1){
		//cout << vec3ToString(P) << endl;
		ParticleEmitter* newEmitter = new ParticleEmitter(P + vec3(0, 10 + ((int)totalTime) % 60, 0), 10, vec3(rand() % 1600 - 800, rand() % 800, rand() % 1600 - 800), 3.0f, 3, 3);
		newEmitter->setColour(vec4(rand() % 80 * 0.01f + 0.2f, rand() % 80 * 0.01f + 0.2f, rand() % 80 * 0.01f + 0.2f, 1));
		newEmitter->emitSpeed = 3.5f;
		static int AddedParticleCounter = 1;
		std::ostringstream os;
		os << AddedParticleCounter++;

		particManager->add(os.str(), newEmitter, "particles\\bouncyball3x3.png");
	}
}

bool initialise()
{
	// Hide the cursor
	glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// Set Key Callback Listener
	glfwSetKeyCallback(renderer::get_window(), keyListener);

	// Set Mouse Callback Listener
	glfwSetMouseButtonCallback(renderer::get_window(), mouseListener);

	// Get the cursor's initial position
	glfwGetCursorPos(renderer::get_window(), &cursor_x, &cursor_y);

	
	// Set the perspective render quality
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // 4-byte pixel alignment
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	// Set the clear colour
	glClearColor(0,0,0,0);

	Util::init();

	SPGrid::getInstance().init(20, 7, 20, 10, vec3(0, 0, 0));
	SPGrid::getInstance().setBasePos(0, -6, 0);
	
	return true;
}

bool load_content()
{
	// *************
	// Load in models
	// *************

	meshes["sphere"] = Util::loadModel("primitives\\sphere.obj");
	meshes["cube"] = Util::loadModel("primitives\\cube.obj");
	meshes["ground"] = Util::loadModel("primitives\\plane.obj");

	skyBox[0] = Util::loadModel("skybox.obj");
	skyBox[1] = skyBox[0];
	skyBox[2] = skyBox[0];

	// ***************
	// Load in textures
	// ***************
	texs["brown"] = Util::loadTexture("brown.jpg");
	
	texs["skybox0"] = Util::loadTexture("skybox\\skybox_texture.jpg");
	texs["skybox1"] = Util::loadTexture("skybox\\skybox_clouds0.png");
	texs["skybox2"] = Util::loadTexture("skybox\\skybox_clouds1.png");

	texs["red"] = Util::loadTexture("solidRed.jpg");
	texs["white"] = Util::loadTexture("white.jpg");
	texs["rock"] = Util::loadTexture("rock.jpg");
	texs["rock-normal"] = Util::loadTexture("rock-normal.jpg");
	texs["ground"] = Util::loadTexture("ground.jpg");

	texs["menuon"] = Util::loadTexture("buttons\\buttons.png");
	texs["menuoff"] = Util::loadTexture("buttons\\off.png");

	// Set up IK
	ikManager = IKManager();
	ikManager["walkingMan"] = IKHierarchy("..\\resources\\ik\\walkingMan.json");
	ikManager["walkingMan"].rootBone->toRender = false;
	ikManager["reachingMan"] = IKHierarchy("..\\resources\\ik\\walkingMan.json");
	ikManager["reachingMan"].rootBone->toRender = false;
	ikManager["reachingMan"].rootBone->origin = vec3(-20, 0, 20);

	// IK for testing reach
	ikManager["reachTester0"] = IKHierarchy("..\\resources\\ik\\reachTester.json");
	ikManager["reachTester0"].rootBone->origin = vec3(23, 10, 25);

	ikManager["reachTester1"] = IKHierarchy("..\\resources\\ik\\reachTester.json");
	ikManager["reachTester1"].rootBone->origin = vec3(24, 10, 25);

	ikManager["reachTester2"] = IKHierarchy("..\\resources\\ik\\reachTester.json");
	ikManager["reachTester2"].rootBone->origin = vec3(25, 10, 25);

	ikManager["reachTester3"] = IKHierarchy("..\\resources\\ik\\reachTester.json");
	ikManager["reachTester3"].rootBone->origin = vec3(26, 10, 25);

	ikManager["reachTester4"] = IKHierarchy("..\\resources\\ik\\reachTester.json");
	ikManager["reachTester4"].rootBone->origin = vec3(27, 10, 25);

	// Initialise the particle Manager
	initParticleManager();

	// Initialise the text renderer
	textRen = new TextRenderer("Quikhand\\font", &passThroughEffect);
	textRen->setFontSize(10.0f);

	// Initialise the graph renderer
	graphRen = new GraphRenderer(textRen, &colourPassThroughEffect, 200.0f, 100.0f);
	graphRen->setText("fps", "time");
	graphRen->setLimit(80);
	graphRen->setBarColour(vec4(1, 1, 1, 1));
	graphRen->setLineColour(vec4(1, 0.3f, 0.3f, 1));

	// Set up the scene
	initSceneObjects();

	// Set up the menu background quad
	initMenuQuad();

	// Set up the shaders
	initShaders();

	// Initialise Cameras
	initCameras();

	cout << endl << endl << "F1: Toggle wireframe + parenting" << endl;
	cout << "F2-4: Switch camera" << endl;
	cout << "F5: Toggle lighting mode" << endl << endl << endl;

	return true;
}

// Initialise the particle manager
void initParticleManager(){
	// Check if there is already a manager, if there is, remove it
	if (particManager)
		delete particManager;

	// Set up the particle manager
	particManager = new ParticleEmitterManager();

	// Create the water emitter
	ParticleEmitter* waterEmitter = new ParticleEmitter(vec3(40, 40, 0), 250, vec3(800, 0, 20), 5.0f, 3, 3);
	waterEmitter->setColour(vec4(0.265, 0.7, 0.8, 1));
	waterEmitter->emitSpeed = 30.0f;
	particManager->add("waterParticles", waterEmitter, "particles\\water3x3.png");

	// Create the tornado emitter
	TornadoParticleEmitter* tornadoEmitter = new TornadoParticleEmitter(vec3(20, 15, -30), 500, vec3(0, 20, 0), 5.0f, 3, 3);
	tornadoEmitter->setColour(vec4(0.15f, 0.15f, 0.15f, 1));
	tornadoEmitter->emitSpeed = 160.0f;
	particManager->add("tornadoParticles", tornadoEmitter, "particles\\smoke3x3.png");
}

// Initialise the shaders
void initShaders(){
	// Add Shaders
	mainEffect.add_shader("..\\resources\\shaders\\main_shader\\main_shader.vert", GL_VERTEX_SHADER);
	mainEffect.add_shader("..\\resources\\shaders\\main_shader\\main_shader.frag", GL_FRAGMENT_SHADER);

	passThroughEffect.add_shader("..\\resources\\shaders\\passthrough_shaders\\texture_passthrough.vert", GL_VERTEX_SHADER);
	passThroughEffect.add_shader("..\\resources\\shaders\\passthrough_shaders\\texture_passthrough.frag", GL_FRAGMENT_SHADER);
		
	colourPassThroughEffect.add_shader("..\\resources\\shaders\\passthrough_shaders\\colour_passthrough.vert", GL_VERTEX_SHADER);
	colourPassThroughEffect.add_shader("..\\resources\\shaders\\passthrough_shaders\\colour_passthrough.frag", GL_FRAGMENT_SHADER);

	// Build effect
	mainEffect.build();
	passThroughEffect.build();
	colourPassThroughEffect.build();

}

// Initialise the menu background quad
void initMenuQuad(){
	// Create the Quad for rendering the menu background to
	float ratio = (50.0f / 350.0f);
	double halfW = renderer::get_screen_width() / 2.0f;
	double halfH = renderer::get_screen_height() / 2.0f;
	geometry menuQuadGeom = geometry();
	vector<vec3> positionsMenuQuad
	{
		vec3(-halfW, -halfH + (halfH*2.0f* ratio), 0.0f),//
		vec3((halfW*2.0f*ratio), -halfH, 0.0f),
		vec3((halfW*2.0f*ratio), -halfH + (halfH*2.0f* ratio), 0.0f),//

		vec3(-halfW, -halfH + (halfH*2.0f* ratio), 0.0f),//
		vec3(-halfW, -halfH, 0.0f),
		vec3((halfW*2.0f*ratio), -halfH, 0.0f)
	};
	vector<vec3> texCoords
	{
		vec3(0.0f, 1.0f, 0.0f),
		vec3(1.0f, 0.0f, 0.0f),
		vec3(1.0f, 1.0f, 0.0f),

		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 0.0f, 0.0f),
		vec3(1.0f, 0.0f, 0.0f)
	};
	// Create the mesh for rendering the menu to
	menuQuadGeom.add_buffer(positionsMenuQuad, BUFFER_INDEXES::POSITION_BUFFER);
	menuQuadGeom.add_buffer(texCoords, BUFFER_INDEXES::TEXTURE_COORDS_0);
	menuQuad = menuQuadGeom;
}

// Initialise the scene objects
void initSceneObjects(){

	// ***************
	// Set up SceneObjects
	// ***************
	
	sceneObjects["cubeA"] = meshes["cube"];
	sceneObjects["cubeA"].set_texture(texs["rock"]); // Sets the texture
	sceneObjects["cubeA"].set_normal_texture(texs["rock-normal"]); // Sets the normal texture
	sceneObjects["cubeA"].set_material(vec4(0.25, 0.25, 0.25, 1), // Sets the material properties
		vec4(0.7, 0.7, 0.7, 1),
		vec4(1, 1, 1, 1),
		50.0f);
	// Set the collider
	sceneObjects["cubeA"].setCollider(new CubeCollider(vec3(-21, 15, 0), vec3(1.0, 1.0, 1.0), ColliderTypes::OBBCUBE));



	sceneObjects["ground"] = meshes["ground"];
	sceneObjects["ground"].set_texture(texs["ground"]); // Sets the texture
	sceneObjects["ground"].set_material(vec4(0.25, 0.25, 0.25, 1), // Sets the material properties
		vec4(0.7, 0.7, 0.7, 1),
		vec4(1, 1, 1, 1),
		50.0f);
	sceneObjects["ground"].setCollider(new PlaneCollider(vec3(0, 0, 0), vec3(0, 1, 0)));
	sceneObjects["ground"].getCollider()->staticPos = true;

	sceneObjects["cubeB"] = meshes["cube"];
	sceneObjects["cubeB"].set_texture(texs["white"]); // Sets the texture
	sceneObjects["cubeB"].set_material(vec4(0.25, 0.25, 0.25, 1), // Sets the material properties
		vec4(0.7, 0.7, 0.7, 1),
		vec4(1, 1, 1, 1),
		50.0f);
	// Set the collider
	sceneObjects["cubeB"].setCollider(new CubeCollider(vec3(-21, 10, 0), vec3(1.0, 1.0, 1.0), ColliderTypes::OBBCUBE));


	sceneObjects["cubeC"] = meshes["cube"];
	sceneObjects["cubeC"].set_texture(texs["white"]); // Sets the texture
	sceneObjects["cubeC"].set_material(vec4(0.25, 0.25, 0.25, 1), // Sets the material properties
		vec4(0.7, 0.7, 0.7, 1),
		vec4(1, 1, 1, 1),
		50.0f);
	// Set the collider
	sceneObjects["cubeC"].setCollider(new CubeCollider(vec3(40, 20, 20), vec3(1.0, 1.0, 1.0), ColliderTypes::OBBCUBE));

	sceneObjects["sphereA"] = meshes["sphere"];
	sceneObjects["sphereA"].set_texture(texs["white"]); // Sets the texture
	sceneObjects["sphereA"].set_material(vec4(0.25, 0.25, 0.25, 1), // Sets the material properties
		vec4(0.7, 0.7, 0.7, 1),
		vec4(1, 1, 1, 1),
		50.0f);
	// Set the collider
	sceneObjects["sphereA"].setCollider(new CubeCollider(vec3(-40, 30, -20), vec3(1.0, 1.0, 1.0), ColliderTypes::OBBCUBE));

	sceneObjects["reachTesterTarget"] = meshes["sphere"];
	sceneObjects["reachTesterTarget"].get_transform().scale = vec3(0.2f,0.2f,0.2f);
	sceneObjects["reachTesterTarget"].set_texture(texs["red"]); // Sets the texture
	sceneObjects["reachTesterTarget"].set_material(vec4(0.25, 0.25, 0.25, 1), // Sets the material properties
		vec4(0.7, 0.7, 0.7, 1),
		vec4(1, 1, 1, 1),
		50.0f);
	// Set the collider
	sceneObjects["reachTesterTarget"].get_transform().position = vec3(25,15,25);

	sceneObjects["reachingManTarget"] = sceneObjects["reachTesterTarget"];
	
}

// Initialise the cameras
void initCameras(){
	// Set camera properties
	auto aspect = static_cast<float>(renderer::get_screen_width()) / static_cast<float>(renderer::get_screen_height());
	
	//Target Camera
	targetCam.set_position(vec3(100.0f, 70.0f, 100.0f));
	targetCam.set_target(vec3(0.0f, 0.0f, 0.0f));
	targetCam.set_projection(quarter_pi<float>(), aspect, MYNEAR, MYFAR);

	// Free Camera
	freeCam.set_position(vec3(0.0f, 5.0f, 30.0f));
	freeCam.set_projection(quarter_pi<float>(), aspect, MYNEAR, MYFAR);

	// MVP for post processing (Orthographics projection)
	float w = renderer::get_screen_width() / 2.0f;
	float h = renderer::get_screen_height() / 2.0f;
	mat4 orthoProj = ortho<float>(-w, w, -h, h, -10, 20);
	mat4 orthoView = lookAt(vec3(0, 0, 1), vec3(0, 0, 0), vec3(0, 1, 0));

	orthoMVP = orthoProj * orthoView * mat4(1.0);

}

void updateCameras(float delta_time)
{
	/*
		Gets the current mouse position
	*/
	double current_x;
	double current_y;
	glfwGetCursorPos(renderer::get_window(), &current_x, &current_y);
	
	/*
		Calculate the delta value from the last frame's cursor values
	*/
	double delta_x = 0;
	double delta_y = 0;
	
	if (glfwGetMouseButton(renderer::get_window(), GLFW_MOUSE_BUTTON_2)){
		delta_x = current_x - cursor_x;
		delta_y = cursor_y - current_y;
		glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else{
		glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}


	/*
		Multiply the delta values by the screen ratios to get the correct change in mouse position
	*/
	double ratioX = (double)renderer::get_screen_width() / (double)renderer::get_screen_height();
	double ratioY = 1 / ratioX;

	delta_x *= ratioX;
	delta_y *= ratioY;


	// Update the camera
	switch (currentCamera){
	case Camera::Target:
			{

			  vec3 p = targetCam.get_position();

			  mat4 rotationMat(1);
			  
			  if (glfwGetKey(renderer::get_window(), GLFW_KEY_A)){
				  targetCamDirection = -1;
				  rotationMat = rotate(rotationMat, -(quarter_pi<float>())*delta_time, vec3(0.0, 1.0, 0.0));
			  }
			  if (glfwGetKey(renderer::get_window(), GLFW_KEY_D)){
				  targetCamDirection = 1;
				  rotationMat = rotate(rotationMat, (quarter_pi<float>())*delta_time, vec3(0.0, 1.0, 0.0));
			  }

			  rotationMat = rotate(rotationMat, targetCamDirection * (quarter_pi<float>()/2)*delta_time, vec3(0.0, 1.0, 0.0));


			  p = vec3(rotationMat * vec4(p, 1.0f));
			  targetCam.set_position(p);
			  targetCam.update(delta_time); 

			  camPos = targetCam.get_position();
			}
		break;
	case Camera::Free:


		/*
			Rotate the camera relative to the delta values
		*/
		freeCam.rotate((float)delta_x / 360.0f, (float)delta_y / 360.0f);

		/*
			Handle the camera movement using key presses
		*/

		float scale = 1.0f;
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_LEFT_SHIFT))
			scale = 4.0f;

		if (glfwGetKey(renderer::get_window(), GLFW_KEY_W))
			freeCam.move(vec3(0, 0, 40) * scale * delta_time);
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_S))
			freeCam.move(vec3(0, 0, -40) * scale * delta_time);
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_A))
			freeCam.move(vec3(-40, 0, 0) * scale * delta_time);
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_D))
			freeCam.move(vec3(40, 0, 0) * scale * delta_time);
		
		freeCam.update(delta_time);

		camPos = freeCam.get_position();
		break;
	}

	/*
		Update the cursor position
	*/
	cursor_x = current_x;
	cursor_y = current_y;

}

void updatePhysics(){
	// While the accumulated delta time is greater than the physics time step, run the physics update and decrement accumDeltaTime by PHYSICS_TIME_STEP
	while (accumDeltaTime > PHYSICS_TIME_STEP){
		accumDeltaTime -= PHYSICS_TIME_STEP;

		totalPhysicsTime += PHYSICS_TIME_STEP;

		// Cube movement
		float movementForce = 20.0f;
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_UP))
			sceneObjects["cubeA"].getCollider()->addForce(vec3(movementForce, 0.0, 0.0));

		if (glfwGetKey(renderer::get_window(), GLFW_KEY_DOWN))
			sceneObjects["cubeA"].getCollider()->addForce(vec3(-movementForce, 0.0, 0.0));

		if (glfwGetKey(renderer::get_window(), GLFW_KEY_LEFT))
			sceneObjects["cubeA"].getCollider()->addForce(vec3(0.0, 0.0, -movementForce));

		if (glfwGetKey(renderer::get_window(), GLFW_KEY_RIGHT))
			sceneObjects["cubeA"].getCollider()->addForce(vec3(0.0, 0.0, movementForce));

		if (glfwGetKey(renderer::get_window(), GLFW_KEY_E))
			sceneObjects["cubeA"].getCollider()->addForce(vec3(0.0, movementForce + 9.8, 0.0));

		if (glfwGetKey(renderer::get_window(), GLFW_KEY_Q))
			sceneObjects["cubeA"].getCollider()->addForce(vec3(0.0, -movementForce, 0.0));

		if (glfwGetKey(renderer::get_window(), GLFW_KEY_P))
			attemptToMakeAParticleEmitter();

		// refresh the spatial partioning grid with the sceneobjects
		SPGrid::getInstance().update(sceneObjects);

		if (toggleIK){
			// Update IK
			ikManager.update();

			// IK Demonstration updating - START
			ikManager["walkingMan"].endLinks["leftFoot"]->reach(ikManager["walkingMan"].rootBone->origin + vec3(sin(totalPhysicsTime), fmax(0.0f, sin(totalPhysicsTime)), -0.4), PHYSICS_TIME_STEP);
			ikManager["walkingMan"].endLinks["rightFoot"]->reach(ikManager["walkingMan"].rootBone->origin + vec3(-sin(totalPhysicsTime), fmax(0.0f, sin(totalPhysicsTime)), 0.4), PHYSICS_TIME_STEP);
			ikManager["walkingMan"].endLinks["lowerArmLeft"]->reach(ikManager["walkingMan"].rootBone->origin + vec3(-sin(totalPhysicsTime)*2.0f, 1.8f + fmax(0.0f, sin(totalPhysicsTime)), -0.55), PHYSICS_TIME_STEP);
			ikManager["walkingMan"].endLinks["lowerArmRight"]->reach(ikManager["walkingMan"].rootBone->origin + vec3(sin(totalPhysicsTime)*2.0f, 1.8f + fmax(0.0f, sin(totalPhysicsTime)), 0.55), PHYSICS_TIME_STEP);

			sceneObjects["reachingManTarget"].get_transform().position = ikManager["reachingMan"].rootBone->origin + vec3(5, 5 + sin(totalPhysicsTime*2.0f) * 2.0f, sin(totalPhysicsTime*0.8f) * 4.0f);
			ikManager["reachingMan"].endLinks["leftHand"]->reach(sceneObjects["reachingManTarget"].get_transform().position, PHYSICS_TIME_STEP);

			ikManager["reachTester0"].endLinks["End"]->reach(sceneObjects["reachTesterTarget"].get_transform().position, PHYSICS_TIME_STEP);
			ikManager["reachTester1"].endLinks["End"]->reach(sceneObjects["reachTesterTarget"].get_transform().position, PHYSICS_TIME_STEP);
			ikManager["reachTester2"].endLinks["End"]->reach(sceneObjects["reachTesterTarget"].get_transform().position, PHYSICS_TIME_STEP);
			ikManager["reachTester3"].endLinks["End"]->reach(sceneObjects["reachTesterTarget"].get_transform().position, PHYSICS_TIME_STEP);
			ikManager["reachTester4"].endLinks["End"]->reach(sceneObjects["reachTesterTarget"].get_transform().position, PHYSICS_TIME_STEP);
			// IK Demonstration updating - END
		}
		
		// Loop through the sceneobjects and update them
		for (auto &e : sceneObjects)
		{
			e.second.update(PHYSICS_TIME_STEP);
		}
		
		// Update the particle manager
		if (toggleParticles)particManager->update(PHYSICS_TIME_STEP);

	}
}



bool update(float delta_time)
{
	// Increment accumDeltaTime
	accumDeltaTime += delta_time;
	updatePhysics();

	// Increment totalTime
	totalTime += delta_time;
	float fps = 1.0f / delta_time;
	// Push fps data to the graph renderer
	graphRen->pushData(fps);
	DBG_ASSERT(graphRen->getAverageDataValue() > 15.0f);

	// IK target movement
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_I))
		sceneObjects["reachTesterTarget"].get_transform().position += vec3(0, 0, 10) * delta_time;

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_K))
		sceneObjects["reachTesterTarget"].get_transform().position += vec3(0, 0, -10) * delta_time;

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_J))
			sceneObjects["reachTesterTarget"].get_transform().position += vec3(10, 0, 0) * delta_time;

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_L))
			sceneObjects["reachTesterTarget"].get_transform().position += vec3(-10, 0, 0) * delta_time;

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_T))
		sceneObjects["reachTesterTarget"].get_transform().position += vec3(0, 10, 0) * delta_time;

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_Y))
		sceneObjects["reachTesterTarget"].get_transform().position += vec3(0, -10, 0) * delta_time;

	updateCameras(delta_time);

	//Rotate the skyboxes (part of this method since the skybox contains the sun)
	skyBox[1].get_transform().rotate(vec3(0, (quarter_pi<float>() / 12.0f)*delta_time, 0));
	skyBox[2].get_transform().rotate(vec3(0, -(quarter_pi<float>() / 12.0f)*delta_time, 0));

	return true;
}

// Renders a SceneObject and it's children
void renderSceneObjects(const mat4 &VP){
	// Render the SceneObjects
	for (auto& mapObj : sceneObjects){
		mapObj.second.render(VP, mainEffect);
	}
}

bool render()
{

	// Set up the view, projection and camera direction
	mat4 tempV, tempP;
	vec3 camDir;
	switch (currentCamera){
	case Camera::Target:
		tempV = targetCam.get_view();
		tempP = targetCam.get_projection();
		camDir = normalize(targetCam.get_target() - targetCam.get_position())*vec3(-1, -1, -1);
		break;
	case Camera::Free:
		tempV = freeCam.get_view();
		tempP = freeCam.get_projection();
		camDir = normalize(freeCam.get_target() - freeCam.get_position())*vec3(-1, -1, -1);
		break;
	}


	mat4 VP = tempP * tempV;

	// Bind the main framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// Clear the main frame buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Check whether to render in wireframe or not
	if (isWireframe)
		glPolygonMode(GL_FRONT, GL_LINE);
	else
		glPolygonMode(GL_FRONT, GL_FILL);


	// Render scene normally

	// Set the skybox positions relative to the current camera
	switch (currentCamera){
	case Camera::Target:
		skyBox[0].get_transform().position = targetCam.get_position();
		skyBox[1].get_transform().position = targetCam.get_position();
		skyBox[2].get_transform().position = targetCam.get_position();
		break;
	case Camera::Free:
		skyBox[0].get_transform().position = freeCam.get_position();
		skyBox[1].get_transform().position = freeCam.get_position();
		skyBox[2].get_transform().position = freeCam.get_position();
		break;
	}

	// Bind the passthrough shader
	renderer::bind(passThroughEffect);

	// Disable depth testing
	glDisable(GL_DEPTH_TEST);

	// Render each sky box
	for (int i = 0; i < 3; i++){
		mat4 M = skyBox[i].get_transform().get_transform_matrix();
		mat4 MVP = VP * M;
		glUniformMatrix4fv(
			passThroughEffect.get_uniform_location("MVP"), // Location of uniform
			1, // Number of values - 1 mat4
			GL_FALSE, // Transpose the matrix?
			value_ptr(MVP)); // Pointer to matrix data
		renderer::bind(texs["skybox" + to_string(i)], 0);
		glUniform1i(passThroughEffect.get_uniform_location("tex"), 0);
		renderer::render(skyBox[i]);
	}

	// Re-enable depth testing
	glEnable(GL_DEPTH_TEST);

	// Bind the main effect
	renderer::bind(mainEffect);

	// assign global uniform values
	glUniform3fv(mainEffect.get_uniform_location("ambientLightDir"), 1, value_ptr(normalize(ambientLightPosition)));
	glUniform3fv(mainEffect.get_uniform_location("eyeDir"), 1, value_ptr(camDir));
	glUniform1f(mainEffect.get_uniform_location("totalTime"), totalTime);
	
	// Render the list of SceneObjects
	renderSceneObjects(VP);


	// Render IK
	if (toggleIK){
		renderer::bind(texs["white"], 0);
		ikManager.render(VP, &mainEffect);
	}

	renderer::bind(texs["white"], 0);
	renderer::bind(sceneObjects["cubeA"].get_material(), "mat");

	// Render the particles
	if(toggleParticles)particManager->render(VP);


	// Create lineA and lineB, test for collision 
	LineCollider lineA = LineCollider(vec3(12.5f, 7.5f, 10), vec3(7.5f, 7.5f, 10), 0.0f);
	LineCollider lineB = LineCollider(vec3(9 + sin(totalPhysicsTime)*2.0f, 5, 10), vec3(9 + sin(totalPhysicsTime)*2.0f, 10, 10), 0.0f);

	IntersectionData lineIntersectionData = IntersectionData();
	lineA.intersects(lineB, vec3(0, 0, 0), lineIntersectionData);

	vec3 right = normalize(cross(normalize(freeCam.get_target() - freeCam.get_position()), normalize(freeCam.get_up())));

	// Render Object Controls

	textRen->render3D(VP, right, "T = up  _  Y = down  _  IJKL = forward  left  back  right", sceneObjects["reachTesterTarget"].get_transform().position + vec3(0,1,0));
	textRen->render3D(VP, right, "Q = up  _  E = down  _  arrow keys = forwardleft  back  right", sceneObjects["cubeA"].get_transform().position + vec3(0, 2, 0));
	if (lineIntersectionData.doesIntersect)
		textRen->render3D(VP, right, "Line Intersection: vec3(" + vec3ToString(lineIntersectionData.intersection) + ")", lineIntersectionData.intersection);
	textRen->render3D(VP, right, "Line Intersection Test", lineA.position + (lineA.endPosition - lineA.position)*0.4f + vec3(0, 3, 0));
	textRen->render3D(VP, right, "IK Reach Testers", ikManager["reachTester0"].rootBone->origin + vec3(0, 3, 0));

	renderer::bind(colourPassThroughEffect);
	//glPointSize(6.0f);
	// Set MVP matrix uniform
	glUniformMatrix4fv(
		colourPassThroughEffect.get_uniform_location("MVP"), // Location of uniform
		1, // Number of values - 1 mat4
		GL_FALSE, // Transpose the matrix?
		value_ptr(VP)); // Pointer to matrix data
	

	glUniform4fv(colourPassThroughEffect.get_uniform_location("colour"), 1, value_ptr(vec4(1, 0, 1, 1)));
	// Render the line v line results (demonstrates line v line collision)
	glBegin(GL_LINES);
	glVertex3f(lineA.position.x, lineA.position.y, lineA.position.z);
	glVertex3f(lineA.endPosition.x, lineA.endPosition.y, lineA.endPosition.z);
	glVertex3f(lineB.position.x, lineB.position.y, lineB.position.z);
	glVertex3f(lineB.endPosition.x, lineB.endPosition.y, lineB.endPosition.z);
	glEnd();

	// Render the reach testers debug info
	ikManager["reachTester0"].rootBone->debugRender(VP, colourPassThroughEffect, sceneObjects["reachTesterTarget"].get_transform().position);
	ikManager["reachTester1"].rootBone->debugRender(VP, colourPassThroughEffect, sceneObjects["reachTesterTarget"].get_transform().position);
	ikManager["reachTester2"].rootBone->debugRender(VP, colourPassThroughEffect, sceneObjects["reachTesterTarget"].get_transform().position);
	ikManager["reachTester3"].rootBone->debugRender(VP, colourPassThroughEffect, sceneObjects["reachTesterTarget"].get_transform().position);
	ikManager["reachTester4"].rootBone->debugRender(VP, colourPassThroughEffect, sceneObjects["reachTesterTarget"].get_transform().position);

	// Render the spatial partitioning grid
	if (toggleSPGrid)
		SPGrid::getInstance().render(colourPassThroughEffect, toggleFullSPGrid);

	
	// Disable wireframe
	glPolygonMode(GL_FRONT, GL_FILL);

		
	// Finish rendering this frame
	finishFrame();

	return true;
}

// Finish rendering the frame
void finishFrame(){

	glDisable(GL_DEPTH_TEST);
	
	// Set the current shader to the pass-through shader
	renderer::bind(passThroughEffect);
	
	glUniformMatrix4fv(
		passThroughEffect.get_uniform_location("MVP"), // Location of uniform
		1, // Number of values - 1 mat4
		GL_FALSE, // Transpose the matrix?
		value_ptr(orthoMVP)); // Pointer to matrix data
	
	// Render the menu onto the screen
	if (toggleDebugMenu){
		renderer::bind(texs["menuon"], 2);
		glUniform1i(passThroughEffect.get_uniform_location("tex"), 2);
		renderer::render(menuQuad);

		textRen->setFontSize(7.0f);

		// Render the debug menu text
		textRen->render(orthoMVP, "Toggle\nParticles", 0.0125f, 0.925f);
		textRen->render(orthoMVP, "Toggle\nIK", 0.1f, 0.925f);
		textRen->render(orthoMVP, "Toggle\nFull Grid", 0.189f, 0.925f);
		textRen->render(orthoMVP, "Toggle\nSPGrid", 0.29f, 0.925f);
		textRen->render(orthoMVP, "Reset\nParticles", 0.375f, 0.925f);

		textRen->setFontSize(10.0f);
	}
	else{
		renderer::bind(texs["menuoff"], 2);
		glUniform1i(passThroughEffect.get_uniform_location("tex"), 2);
		renderer::render(menuQuad);
	}

	// Render particle debug info
	if (toggleParticles){
		std::ostringstream avgStr;
		avgStr << particManager->getParticleCount();
		textRen->render(orthoMVP, "Particle Count: " + avgStr.str(), 0, 0.85f);
	}

	// Render camera controls
	textRen->render(orthoMVP, "WASD + Right Click to move the camera", 1.0f - textRen->getStringWidth("WASD + Right Click to move the camera") *1.1f / renderer::get_screen_width(), 0.94f);
	textRen->render(orthoMVP, "Left click to add one emitter, hold 'P' to add many", 1.0f - textRen->getStringWidth("Left click to add one emitter, hold 'P' to add many")*1.1f / renderer::get_screen_width(), 0.0f);

	// Render the fps graph
	graphRen->render(orthoMVP, 0.005f, 0.01f);

	glEnable(GL_DEPTH_TEST);
}

void main()
{
	// Create application
	app application;
	// Set load content, update and render methods
	application.set_load_content(load_content);
	application.set_initialise(initialise);
	application.set_update(update);
	application.set_render(render);

	// Run application
	application.run();
}