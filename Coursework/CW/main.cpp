/*
	Grant Smith (40111906)

	The main class handles the loading in of models and textures, and the updating and rendering of the SceneObjects

*/

#include "main.h"

// Camera Variables
target_camera targetCam;
chase_camera chaseCam;
free_camera freeCam;
vec3 camPos; // The position of the current camera

map<string, mesh> meshes; // A map containing all of the meshes
map<string, SceneObject> sceneObjects; // A map containing all of the main sceneobjects
map<string, texture> texs; // A map containing all of the textures

mesh skyBox[3]; // The meshs for the skybox


bool isWireframe = false; // Whether or not to render in wireframe

vec3 ambientLightPosition = vec3(1800,2500,0); // Set the position of the ambient light

double cursor_x = 0.0; // Stores the position X of the cursor
double cursor_y = 0.0; // Stores the position Y of the cursor

float totalTime = 0.0f; // Stores how much time has passed (used for time based events like model bobbing and light movement)

IntersectionData rayTest;

//TODO
//SphereCollider sphereA = SphereCollider(vec3(0, 100, 2.0), 1.0);
//SphereCollider sphereB = SphereCollider(vec3(0, 100, 0.0), 1.0);
CubeCollider sphereB = CubeCollider(vec3(30, 101, 0.0f), vec3(1.0, 1.0, 1.0), ColliderTypes::OBBCUBE);
CubeCollider sphereA = CubeCollider(vec3(40, 110, 2.0f), vec3(1.0, 1.0, 1.0), ColliderTypes::OBBCUBE);
IntersectionData dataTODO;


// IK constants
map<string, Link*> endLinks;
//TODO

void keyListener(GLFWwindow* window, int key, int scancode, int action, int mods){

	if (key == GLFW_KEY_F1 && action == GLFW_PRESS){
		isWireframe = !isWireframe;
	}
	else  if (key == GLFW_KEY_F2 && action == GLFW_PRESS){
		currentCamera = Camera::Chase;
	}
	else if (key == GLFW_KEY_F3 && action == GLFW_PRESS){
		currentCamera = Camera::Target;
	}
	else if (key == GLFW_KEY_F4 && action == GLFW_PRESS){
		currentCamera = Camera::Free;
	}
	else if (key == GLFW_KEY_F5 && action == GLFW_PRESS){
		//TODO
	}
	else if (key == GLFW_KEY_F6 && action == GLFW_PRESS){
		ssaoOnly = !ssaoOnly;
		ssaoEnabled = true;
	}
	else if (key == GLFW_KEY_F7 && action == GLFW_PRESS){
		glowEnabled = !glowEnabled;
	}
	else if (key == GLFW_KEY_H && action == GLFW_PRESS){
		toggleDebugMenu = !toggleDebugMenu;
	}
}

void mouseListener(GLFWwindow* window, int button, int action, int mods){

	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS){

		//pair<vec3, vec3> posAndDir = Util::screenToWorld
		vec3 pos = freeCam.get_position();
		vec3 camDir = normalize(freeCam.get_target() - freeCam.get_position());
		
		//TODO screenPosToWorldPos
		rayTest = sphereA.rayCast(pos, camDir);


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

			// Loop through to check what button has been pressed
			for (int i = 1; i < 8; i++){
				for (int j = 0; j < 2; j++){
					if (current_x < buttonWidth*i && current_x > buttonWidth*(i - 1)
						&& current_y < 1.0 - buttonHeight*j && current_y > 1.0 - buttonHeight*(j + 1)){
						switch (i + (7 * j) - 1){
						case Buttons::Vignette:
							// Toggle Vignette
							vignetteEnabled = !vignetteEnabled;
							break;
						case Buttons::Motion_Blur:
							// Toggle Motion Blur
							motionBlurEnabled = !motionBlurEnabled;
							break;
						case Buttons::Glow:
							// Toggle Glow
							glowEnabled = !glowEnabled;
							break;
						case Buttons::Grey_Scale:
							// Toggle Greyscale
							greyscaleEnabled = !greyscaleEnabled;
							break;
						case Buttons::Depth_of_Field:
							// Toggle Depth of Field
							//dofEnabled = !dofEnabled;
							break;
						case Buttons::DoF_Debug:
							// Toggle Dof Debug
							//dofEnabled = true;
							//dofDebug = !dofDebug;
							break;
						case Buttons::Free_Cam:
							// Change to the free camera
							currentCamera = Camera::Free;
							break;
						case Buttons::SSAO_Only:
							// Toggle SSAO Only
							if (!ssaoOnly){
								ssaoOnly = true;
								ssaoEnabled = true;
								glowEnabled = false;
								//dofEnabled = false;
								lensFlareEnabled = false;
							}
							else{
								ssaoOnly = false;
								glowEnabled = true;
								//dofEnabled = true;
								lensFlareEnabled = true;
							}
							break;
						case Buttons::SSAO:
							// Toggle SSAO rendering
							ssaoEnabled = !ssaoEnabled;
							break;
						case Buttons::WireFrame:
							// Toggle wireframe mode
							isWireframe = !isWireframe;
							break;
						case Buttons::Lens_Flare:
							// Togle Lens Flare
							lensFlareEnabled = !lensFlareEnabled;
							break;
						case Buttons::Spot_Light:
							// Toggle Spot Light mode
							//TODO 
							break;
						case Buttons::Target_Cam:
							// Change to the target camera
							currentCamera = Camera::Target;
							break;
						case Buttons::Chase_Cam:
							// Change to the chase camera
							currentCamera = Camera::Chase;
							break;
						}
					}
				}
			}
		}

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

	// Set up the required Frame buffers
	postProcessingFbo = FrameBuffer(renderer::get_screen_width(), renderer::get_screen_height());
	fbo = FrameBuffer(renderer::get_screen_width(), renderer::get_screen_height());
	prevFbo = FrameBuffer(renderer::get_screen_width(), renderer::get_screen_height());
	shadowFbo = FrameBuffer(renderer::get_screen_width(), renderer::get_screen_height());
	
	// Set the perspective render quality
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);      // 4-byte pixel alignment
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	// Set the clear colour
	glClearColor(0,0,0,0);

	Util::init();

	

	return true;
}

bool load_content()
{
	// *************
	// Load in models
	// *************
	meshes["island"] = Util::loadModel("island\\island.obj");
	/*
	meshes["barn"] = Util::loadModel("barn\\barn.obj");
	meshes["character"] = Util::loadModel("character\\character.3ds");
	meshes["hay"] = Util::loadModel("hay\\hay.3ds");
	meshes["bridge"] = Util::loadModel("bridge\\bridge.obj");
	meshes["cloth"] = Util::loadModel("cloth\\cloth1.obj");
	meshes["smallRock"] = Util::loadModel("island\\smallRock.obj");
	meshes["largeRock"] = Util::loadModel("island\\largeRock.obj");
	meshes["silo"] = Util::loadModel("silo\\silo.obj");
	meshes["farmhouse"] = Util::loadModel("farmhouse\\farmhouse.obj");
	meshes["glass"] = Util::loadModel("glass\\glass.obj");
	*/
	meshes["sphere"] = Util::loadModel("sphere\\sphere.obj");
	meshes["cube"] = Util::loadModel("cube\\cube.obj");

	skyBox[0] = Util::loadModel("skybox.obj");
	skyBox[1] = skyBox[0];
	skyBox[2] = skyBox[0];

	// ***************
	// Load in textures
	// ***************
	texs["barn"] = Util::loadTexture("barn\\barn.jpg");
	texs["barn-Normal"] = Util::loadTexture("barn\\barn-Normal.jpg");
	texs["island"] = Util::loadTexture("island\\island.jpg");
	texs["island-Normal"] = Util::loadTexture("island\\island-normal.jpg");
	texs["rock"] = Util::loadTexture("rock\\rock.jpg");
	texs["rock-Normal"] = Util::loadTexture("rock\\rock-normal.jpg");
	texs["farmhouse"] = Util::loadTexture("farmhouse\\farmhouse.jpg");
	texs["farmhouse-Normal"] = Util::loadTexture("farmhouse\\farmhouse-Normal.jpg");
	texs["silo"] = Util::loadTexture("silo\\silo.jpg");
	texs["silo-Normal"] = Util::loadTexture("silo\\silo-Normal.jpg");

	texs["hay"] = Util::loadTexture("hay\\hay.png");
	texs["character"] = Util::loadTexture("character\\character.jpg");
	texs["brown"] = Util::loadTexture("brown.jpg");
	texs["orb"] = Util::loadTexture("orb.png");

	texs["glass"] = Util::loadTexture("glass\\glass.png");

	texs["skybox0"] = Util::loadTexture("skybox\\skybox_texture.jpg");
	texs["skybox1"] = Util::loadTexture("skybox\\skybox_clouds0.png");
	texs["skybox2"] = Util::loadTexture("skybox\\skybox_clouds1.png");

	texs["solidRed"] = Util::loadTexture("solidRed.jpg");
	texs["white"] = Util::loadTexture("white.jpg");

	texs["lensflare"] = Util::loadTexture("lensflare\\lensflare.jpg");

	texs["menuon"] = Util::loadTexture("buttons\\buttons.png");
	texs["menuoff"] = Util::loadTexture("buttons\\off.png");
	
	textRen = TextRenderer("Coder's Crux\\font");//TextRenderer("Quikhand\\font");
	textRen.fontScale = 0.3f;

	// Set up the scene
	initSceneObjects();

	// Set up the screen quads
	initScreenQuads();

	// Set up the shaders
	initShaders();

	// Initialise Cameras
	initCameras();

	cout << endl << endl << "F1: Toggle wireframe + parenting" << endl;
	cout << "F2-4: Switch camera" << endl;
	cout << "F5: Toggle lighting mode" << endl << endl << endl;

	return true;
}

void initShaders(){
	// Add Shaders
	mainEffect.add_shader("..\\resources\\shaders\\main_shader\\main_shader.vert", GL_VERTEX_SHADER);
	mainEffect.add_shader("..\\resources\\shaders\\main_shader\\main_shader.frag", GL_FRAGMENT_SHADER);

	passThroughEffect.add_shader("..\\resources\\shaders\\passthrough_shaders\\texture_passthrough.vert", GL_VERTEX_SHADER);
	passThroughEffect.add_shader("..\\resources\\shaders\\passthrough_shaders\\texture_passthrough.frag", GL_FRAGMENT_SHADER);

	depthEffect.add_shader("..\\resources\\shaders\\depth_shader\\depth.vert", GL_VERTEX_SHADER);
	depthEffect.add_shader("..\\resources\\shaders\\depth_shader\\depth.frag", GL_FRAGMENT_SHADER);
	
	colourPassThroughEffect.add_shader("..\\resources\\shaders\\passthrough_shaders\\colour_passthrough.vert", GL_VERTEX_SHADER);
	colourPassThroughEffect.add_shader("..\\resources\\shaders\\passthrough_shaders\\colour_passthrough.frag", GL_FRAGMENT_SHADER);

	postProcessingEffect.add_shader("..\\resources\\shaders\\passthrough_shaders\\texture_passthrough.vert", GL_VERTEX_SHADER);
	postProcessingEffect.add_shader("..\\resources\\shaders\\post_processing_shader\\post_processing_shader.frag", GL_FRAGMENT_SHADER);//ssao_shader.frag", GL_FRAGMENT_SHADER);

	// Build effect
	mainEffect.build();
	passThroughEffect.build();
	depthEffect.build();
	colourPassThroughEffect.build();
	postProcessingEffect.build();

}

void initScreenQuads(){
	// Create the Quad for rendering the screen to
	geometry screenQuadGeom = geometry();
	double halfW = renderer::get_screen_width() / 2.0f;
	double halfH = renderer::get_screen_height() / 2.0f;
	vector<vec3> positions
	{
		vec3(-halfW, halfH, 0.0f),
		vec3(halfW, -halfH, 0.0f),
		vec3(halfW, halfH, 0.0f),

		vec3(-halfW, halfH, 0.0f),
		vec3(-halfW, -halfH, 0.0f),
		vec3(halfW, -halfH, 0.0f)
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
	// Create the mesh for rendering the screen to
	screenQuadGeom.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
	screenQuadGeom.add_buffer(texCoords, BUFFER_INDEXES::TEXTURE_COORDS_0);
	screenQuad = screenQuadGeom;

	// Create the Quad for rendering the corner cam to
	float cornerCamScale = 0.5f;
	geometry cornerCamQuadGeom = geometry();
	vector<vec3> positionsCornerCam
	{
		vec3(-halfW, halfH, 0.0f),
		vec3(-halfW + halfW * cornerCamScale, halfH - halfH * cornerCamScale, 0.0f),
		vec3(-halfW + halfW * cornerCamScale, halfH, 0.0f),

		vec3(-halfW, halfH, 0.0f),
		vec3(-halfW, halfH - halfH * cornerCamScale, 0.0f),
		vec3(-halfW + halfW * cornerCamScale, halfH - halfH * cornerCamScale, 0.0f)
	};
	// Create the mesh for rendering the corner cam to
	cornerCamQuadGeom.add_buffer(positionsCornerCam, BUFFER_INDEXES::POSITION_BUFFER);
	cornerCamQuadGeom.add_buffer(texCoords, BUFFER_INDEXES::TEXTURE_COORDS_0);
	cornerCamQuad = cornerCamQuadGeom;

	// Create the Quad for rendering the menu to
	float ratio = (50.0f / 350.0f);
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
	// Create the mesh for rendering the menu to
	menuQuadGeom.add_buffer(positionsMenuQuad, BUFFER_INDEXES::POSITION_BUFFER);
	menuQuadGeom.add_buffer(texCoords, BUFFER_INDEXES::TEXTURE_COORDS_0);
	menuQuad = menuQuadGeom;
}

void initSceneObjects(){

	//TODO IK INIT

	float scale = 1.0f;

	// Create a bipedal hierarchy
	endLinks["root"] = new Link(vec3(0, 0, 1), -half_pi<float>(), 1.0f*scale);
	endLinks["root"]->origin = vec3(40, 100, 0);
	//endLinks["root"]->toRender = false;

	endLinks["root"]->addChild("waist", new Link(vec3(0, 0, 1), 0.0f, 0.8f*scale));
	endLinks["root"]->children["waist"]->addChild("chest", new Link(vec3(0, 0, 1), 0.0f, 1.4f*scale));
	endLinks["root"]->children["waist"]->children["chest"]->addChild("neck", new Link(vec3(0, 0, 1), 0.0f, 0.3f*scale));

	// Head and Neck
	endLinks["head"] = new Link(vec3(0, 0, 1), 0.0f, 0.5f*scale);
	endLinks["head"]->setParent("head", endLinks["root"]->children["waist"]->children["chest"]->children["neck"]);
	endLinks["head"]->linkReach = 1;

	// Left Arm
	endLinks["root"]->children["waist"]->children["chest"]->addChild("leftShoulder", new Link(normalize(vec3(0, 1, 0)), half_pi<float>(), 0.5f*scale));
	endLinks["root"]->children["waist"]->children["chest"]->children["leftShoulder"]->addChild("upperArm", new Link(normalize(vec3(0, 1, 0)), quarter_pi<float>(), 1.2f*scale));
	endLinks["root"]->children["waist"]->children["chest"]->children["leftShoulder"]->children["upperArm"]->addChild("lowerArm", new Link(normalize(vec3(0, 1, 0)), quarter_pi<float>(), 1.0f*scale));
	endLinks["leftHand"] = new Link(vec3(0, 0, 1), 0.0f, 0.3f*scale);
	endLinks["root"]->children["waist"]->children["chest"]->children["leftShoulder"]->children["upperArm"]->children["lowerArm"]->addChild("hand", endLinks["leftHand"]);

	// Right Arm
	endLinks["root"]->children["waist"]->children["chest"]->addChild("rightShoulder", new Link(normalize(vec3(0, 1, 0)), -half_pi<float>(), 0.5f*scale));
	endLinks["root"]->children["waist"]->children["chest"]->children["rightShoulder"]->addChild("upperArm", new Link(normalize(vec3(0, 1, 0)), -quarter_pi<float>(), 1.2f*scale));
	endLinks["root"]->children["waist"]->children["chest"]->children["rightShoulder"]->children["upperArm"]->addChild("lowerArm", new Link(normalize(vec3(0, 1, 0)), -quarter_pi<float>(), 1.0f*scale));
	endLinks["rightHand"] = new Link(vec3(0, 0, 1), 0.0f, 0.3f*scale);
	endLinks["root"]->children["waist"]->children["chest"]->children["rightShoulder"]->children["upperArm"]->children["lowerArm"]->addChild("hand", endLinks["rightHand"]);

	// Left Leg
	endLinks["root"]->addChild("leftHip", new Link(vec3(0, 1, 0), half_pi<float>(), 0.4f*scale));
	endLinks["root"]->children["leftHip"]->addChild("upperLeg", new Link(normalize(vec3(0, 1, 0)), half_pi<float>(), 1.2f*scale));
	endLinks["root"]->children["leftHip"]->children["upperLeg"]->addChild("lowerLeg", new Link(normalize(vec3(0, 1, 0)), 0.0f, 1.0f*scale));
	endLinks["leftFoot"] = new Link(vec3(0, 0, 1), half_pi<float>(), 0.4f*scale);
	endLinks["root"]->children["leftHip"]->children["upperLeg"]->children["lowerLeg"]->addChild("foot", endLinks["leftFoot"]);

	// Right Leg
	endLinks["root"]->addChild("rightHip", new Link(vec3(0, 1, 0), -half_pi<float>(), 0.4f*scale));
	endLinks["root"]->children["rightHip"]->addChild("upperLeg", new Link(normalize(vec3(0, 1, 0)), -half_pi<float>(), 1.2f*scale));
	endLinks["root"]->children["rightHip"]->children["upperLeg"]->addChild("lowerLeg", new Link(normalize(vec3(0, 1, 0)), 0.0f, 1.0f*scale));
	endLinks["rightFoot"] = new Link(vec3(0, 0, 1), half_pi<float>(), 0.4f*scale);
	endLinks["root"]->children["rightHip"]->children["upperLeg"]->children["lowerLeg"]->addChild("foot", endLinks["rightFoot"]);

	// IK Reach Limits
	endLinks["head"]->linkReach = 1;
	endLinks["leftFoot"]->linkReach = 3;
	endLinks["rightFoot"]->linkReach = 3;
	endLinks["leftHand"]->linkReach = 3;
	endLinks["rightHand"]->linkReach = 3;

	// IK Angle Limits
	//endLinks["root"]->children["waist"]->children["chest"]->children["rightShoulder"]->setAngleLimits(vec3(-pi<float>(), -quarter_pi<float>(), -pi<float>()), vec3(pi<float>(), quarter_pi<float>(), pi<float>()));

	//endLinks["root"]->children["waist"]->children["chest"]->children["rightShoulder"]->priority = 0.2f;
	//endLinks["root"]->children["waist"]->children["chest"]->children["rightShoulder"]->children["upperArm"]->priority = 0.5f;


	/*
	endLinks.push_back(new Link(vec3(0, 0, 1), 0.0f, l));
	endLinks[0]->setParent(new Link(vec3(0, 0, 1), f, l));
	Link* parent = endLinks[0]->parent;
	parent->setParent(new Link(vec3(0, 0, -1), f, l));
	parent->parent->setParent(new Link(vec3(0, 0, -1), half_pi<float>(), l));

	endLinks[0]->getRoot()->origin = vec3(40, 100, 0);
	endLinks[0]->getRoot()->m_length = 2.0f;
	endLinks[0]->linkReach = -1;

	endLinks.push_back(new Link(vec3(0, 0, 1), 0.0f, l));
	Link* ll = new Link(vec3(0, 0, 1), 0.0f, l);
	ll->setParent(endLinks[0]->parent->parent->parent);
	endLinks[1]->setParent(ll);
	endLinks[1]->linkReach = -1;
	*/

	// ***************
	// Set up SceneObjects
	// ***************
	sceneObjects["island"] = meshes["island"]; // Creates a new SceneObject from the mesh
	sceneObjects["island"].set_texture(texs["island"]); // Sets the texture
	sceneObjects["island"].set_normal_texture(texs["island-Normal"]); // Sets the normal texture
	sceneObjects["island"].set_material(vec4(0.25, 0.25, 0.25, 1), // Sets the material properties
		vec4(0.7, 0.7, 0.7, 1),
		vec4(1, 1, 1, 1),
		50.0f);


	sceneObjects["sphereA"] = meshes["cube"];
	sceneObjects["sphereA"].set_texture(texs["island"]); // Sets the texture
	sceneObjects["sphereA"].set_normal_texture(texs["island-Normal"]); // Sets the normal texture
	sceneObjects["sphereA"].set_material(vec4(0.25, 0.25, 0.25, 1), // Sets the material properties
		vec4(0.7, 0.7, 0.7, 1),
		vec4(1, 1, 1, 1),
		50.0f);
	
	sceneObjects["sphereB"] = meshes["cube"];
	sceneObjects["sphereB"].set_texture(texs["island"]); // Sets the texture
	sceneObjects["sphereB"].set_normal_texture(texs["island-Normal"]); // Sets the normal texture
	sceneObjects["sphereB"].set_material(vec4(0.25, 0.25, 0.25, 1), // Sets the material properties
		vec4(0.7, 0.7, 0.7, 1),
		vec4(1, 1, 1, 1),
		50.0f);

	sphereB.rotate(vec3(0,0,1), 45.0f);
	sceneObjects["sphereB"].get_transform().rotate(quat(1.0,0.0,0.0, cos(pi<float>() / 4.0f)));

}

void initCameras(){
	// Set camera properties
	auto aspect = static_cast<float>(renderer::get_screen_width()) / static_cast<float>(renderer::get_screen_height());

	//Chase Camera
	/*
	chaseCam.set_pos_offset(vec3(0.0f, 25.0f, 15.0f));
	chaseCam.set_springiness(0.5f);
	chaseCam.move( (*sceneObjects["island"].get_children())["character"].get_transform_with_parent().position,
		eulerAngles((*sceneObjects["island"].get_children())["character"].get_transform_with_parent().orientation));
	chaseCam.set_projection(quarter_pi<float>(), aspect, MYNEAR, MYFAR);
	*/

	//Target Camera
	targetCam.set_position(vec3(1200.0f, 650.0f, 1200.0f));
	targetCam.set_target(vec3(0.0f, -200.0f, 0.0f));
	targetCam.set_projection(quarter_pi<float>(), aspect, MYNEAR, MYFAR);

	// Free Camera
	freeCam.set_position(vec3(0.0f, 85.0f, 0.0f));
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
	case Camera::Chase:
		{
			// The target object
			static SceneObject &target_mesh = sceneObjects["island"];//.get_children())["character"];


			/*
				Rotate the camera around the y axis
			*/
			chaseCam.rotate(vec3((float)delta_y / 360.0f, 0, 0));


			/*
				rotate target mesh
			*/
			target_mesh.get_transform().rotate(vec3(0, -(float)delta_x / 360.0f, 0));


			/*
				Move the target mesh using keypresses
			*/
			quat ori = inverse(target_mesh.get_transform_with_parent().orientation);

			if (glfwGetKey(renderer::get_window(), GLFW_KEY_W))
				target_mesh.get_transform().translate(vec3(0, 0, -20) * ori * delta_time);
			if (glfwGetKey(renderer::get_window(), GLFW_KEY_S))
				target_mesh.get_transform().translate(vec3(0, 0, 20) * ori * delta_time);
			if (glfwGetKey(renderer::get_window(), GLFW_KEY_A))
				target_mesh.get_transform().translate(vec3(-20, 0, 0) * ori * delta_time);
			if (glfwGetKey(renderer::get_window(), GLFW_KEY_D))
				target_mesh.get_transform().translate(vec3(20, 0, 0) * ori * delta_time);

			target_mesh.update(delta_time);


			/*
				Move the camera to the targets position and orientation
			*/
			chaseCam.move(target_mesh.get_transform_with_parent().position, eulerAngles(target_mesh.get_transform_with_parent().orientation));


			/*
				update the camera
			*/
			chaseCam.update(delta_time);


			camPos = chaseCam.get_position();
		}
		break;
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

void updateLighting(float delta_time)
{
	mat4 rotationMat(1);
	rotationMat = rotate(rotationMat, (quarter_pi<float>() / 12.0f)*delta_time, vec3(0.0, 1.0, 0.0));
	ambientLightPosition = vec3(rotationMat * vec4(ambientLightPosition, 1.0f));

	//Rotate the skyboxes (part of this method since the skybox contains the sun)
	skyBox[1].get_transform().rotate(vec3(0, (quarter_pi<float>() / 12.0f)*delta_time, 0));
	skyBox[2].get_transform().rotate(vec3(0, -(quarter_pi<float>() / 12.0f)*delta_time, 0));
}

//TODO IK
void updateIK(mat4 &proj, mat4 &view){

	mat4 PV = proj*view;
	vec3 target = sphereA.position;
	
	endLinks["root"]->update();
	float physicsTimeStep = 0.1f;
	//endLinks["root"]->reach(target, physicsTimeStep);
	endLinks["root"]->render(PV, colourPassThroughEffect, *endLinks["root"], target);

	//endLinks["leftHand"]->reach(sphereA.position, physicsTimeStep);
	endLinks["rightHand"]->reach(sphereA.position, physicsTimeStep);

	sphereB.position = vec3(50, 101, sin(totalTime)*8.0f);

}




bool update(float delta_time)
{
	totalTime += delta_time;
	


	float velocity = 5.5f;
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_UP))
		sphereA.translate(vec3(velocity, 0.0, 0.0)*delta_time);
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_DOWN))
		sphereA.translate(vec3(-velocity, 0.0, 0.0)*delta_time);
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_LEFT))
		sphereA.translate(vec3(0.0, 0.0, velocity)*delta_time);
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_RIGHT))
		sphereA.translate(vec3(0.0, 0.0, -velocity)*delta_time);

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_E))
		sphereA.translate(vec3(0.0, velocity, 0.0)*delta_time);

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_Q))
		sphereA.translate(vec3(0.0, -velocity, 0.0)*delta_time);

	
	dataTODO = sphereA.intersects(&sphereB, magnitude(vec3(0.0, 0.0, -velocity))*delta_time);
	//sphereA.translate(vec3(0.0,0.0,0.01));
	if (dataTODO.doesIntersect){
		sphereA.translate(dataTODO.direction*-dataTODO.amount);
		cout << "Sphere collision: " << endl;
		cout << dataTODO.doesIntersect << endl;
		cout << dataTODO.direction.x << ", " << dataTODO.direction.y << ", " << dataTODO.direction.z << " * " << dataTODO.amount << endl;
	}

	sceneObjects["sphereA"].get_transform().position = sphereA.position;
	sceneObjects["sphereB"].get_transform().position = sphereB.position;



	// Write the fps to the console
	//cout << int(1.0f / delta_time) << " fps          " << '\r';

	for (auto &e : sceneObjects)
	{
		e.second.update(delta_time);
	}

	updateCameras(delta_time);
	updateLighting(delta_time);

	return true;
}

// Renders a SceneObject and it's children
void renderMesh(SceneObject& sO, const mat4 &V, const mat4 &P){

	mat4 M = sO.get_transform_with_parent().get_transform_matrix();

	mat4 MVP = P * V * M;
	// Set MVP matrix uniform
	glUniformMatrix4fv(
		mainEffect.get_uniform_location("MVP"), // Location of uniform
		1, // Number of values - 1 mat4
		GL_FALSE, // Transpose the matrix?
		value_ptr(MVP)); // Pointer to matrix data

	glUniformMatrix4fv(mainEffect.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));

	// Check if the SceneObject has a normal map
	if (sO._normal){
		// Bind the normal
		renderer::bind(*(sO._normal), 1);
		glUniform1i(mainEffect.get_uniform_location("hasNormalMap"), true);
		glUniform1i(mainEffect.get_uniform_location("normalTex"), 1);
	}
	else{
		glUniform1i(mainEffect.get_uniform_location("hasNormalMap"), false);
	}

	// If the object has a parent and wireframe is enabled then render the parent-child hierarchy
	if (sO._parent && isWireframe){
		// Pass wind uniforms
		vec3 linePos = -sO.get_transform().position / sO.get_transform_with_parent().scale;

		linePos = rotate(inverse(sO.get_transform().orientation), linePos);

		renderer::bind(texs["solidRed"], 0);
		renderer::bind(material(vec4(1, 1, 1, 1), vec4(1, 1, 1, 1), vec4(1, 1, 1, 1), 1), "mat");

		glLineWidth(3);
		glBegin(GL_LINES);
			glVertex3f(0, 0, 0);
			glVertex3f(linePos.x, linePos.y, linePos.z);
		glEnd();
		vec3 objScale = sO.get_transform_with_parent().scale;
		vec3 linePosNorm = normalize(linePos) / objScale;
		glLineWidth(10);
		renderer::bind(texs["white"], 0);
		glBegin(GL_LINES);
			glVertex3f(0, 0, 0);
			glVertex3f(linePosNorm.x, linePosNorm.y, linePosNorm.z);
		glEnd();
		glLineWidth(1);

	}
	
	// Bind and set texture
	renderer::bind((sO.get_texture()), 0);
	glUniform1i(mainEffect.get_uniform_location("tex"), 0);

	// Bind the SceneObject's material
	renderer::bind(sO.get_material(), "mat");

	// Render the current SceneObject
	sO.render();

	// Render the list of child SceneObjects
	for (SceneObject& child : *(sO.get_children())){
		renderMesh(child, V, P);
	}

}

// Renders a SceneObject and it's children
void renderMeshDepth(SceneObject& sO, const mat4 &V, const mat4 &P){

	mat4 M = sO.get_transform_with_parent().get_transform_matrix();

	mat4 MVP = P * V * M;
	// Set MVP matrix uniform
	glUniformMatrix4fv(
		depthEffect.get_uniform_location("MVP"), // Location of uniform
		1, // Number of values - 1 mat4
		GL_FALSE, // Transpose the matrix?
		value_ptr(MVP)); // Pointer to matrix data
	
	// Render the current SceneObject
	sO.render();

	// Render the list of child SceneObjects
	for (SceneObject& child : *(sO.get_children())){
		renderMeshDepth(child, V, P);
	}

}

bool render()
{

	//------------ Start Shadow Map ------------//

	// Sort the SceneObjects into furthest from camera rendered first

	// Bind the shadow framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo.get_buffer());
	// Clear the shadow framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Change culling to front face
	glCullFace(GL_FRONT);

	// Compute the MVP matrix from the light's point of view
	float orthoScale = 1.0f / 8.0f;
	float farOrtho = MYFAR*orthoScale;
	mat4 depthProjectionMatrix = ortho<float>(-farOrtho, farOrtho, -farOrtho, farOrtho, -farOrtho, farOrtho);
	mat4 depthViewMatrix = lookAt(normalize(ambientLightPosition), normalize(-ambientLightPosition), vec3(0, 1, 0));
	
	// Bind the depth shader
	renderer::bind(depthEffect);
	glUniform1f(depthEffect.get_uniform_location("totalTime"), totalTime);
	glUniform1f(depthEffect.get_uniform_location("near"), MYNEAR);
	glUniform1f(depthEffect.get_uniform_location("far"), MYFAR);
	// Render the list of sorted child SceneObjects
	for (auto& mapObj : sceneObjects){
		renderMeshDepth(mapObj.second, depthViewMatrix, depthProjectionMatrix);
	}

	// Change the culling back to back face
	glCullFace(GL_BACK);

	//------------ End Shadow Map ------------//

	// Set up the view, projection and camera direction
	mat4 V, P;
	vec3 camDir;
	switch (currentCamera){
	case Camera::Chase:
		V = chaseCam.get_view();
		P = chaseCam.get_projection();
		camDir = normalize(chaseCam.get_target() - chaseCam.get_position())*vec3(-1, -1, -1);
		break;
	case Camera::Target:
		V = targetCam.get_view();
		P = targetCam.get_projection();
		camDir = normalize(targetCam.get_target() - targetCam.get_position())*vec3(-1, -1, -1);
		break;
	case Camera::Free:
		V = freeCam.get_view();
		P = freeCam.get_projection();
		camDir = normalize(freeCam.get_target() - freeCam.get_position())*vec3(-1, -1, -1);
		break;
	}

	// Bind the main framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo.get_buffer());
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
	case Camera::Chase:
		skyBox[0].get_transform().position = chaseCam.get_position();
		skyBox[1].get_transform().position = chaseCam.get_position();
		skyBox[2].get_transform().position = chaseCam.get_position();
		break;
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
		mat4 MVP = P * V * M;
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

	// Bind the shadow map's depth
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, shadowFbo.get_depth());
	glUniform1i(mainEffect.get_uniform_location("shadowMap"), 3);
	// Set up the depth bias matrix (shifts the coords)
	mat4 biasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
		);
	glUniformMatrix4fv(mainEffect.get_uniform_location("depthBias"), 1, GL_FALSE, value_ptr(biasMatrix));
	glUniformMatrix4fv(mainEffect.get_uniform_location("lightVP"), 1, GL_FALSE, value_ptr(depthProjectionMatrix*depthViewMatrix));

	// Render the list of sorted child SceneObjects
	for (auto& mapObj : sceneObjects){
		renderMesh(mapObj.second, V, P);
	}


	// TODO --------------------
	if (dataTODO.doesIntersect){
		glDisable(GL_DEPTH_TEST);
		renderer::bind(colourPassThroughEffect);
		glUniform4fv(colourPassThroughEffect.get_uniform_location("colour"), 1, value_ptr(vec4(1, 0, 0, 1)));
		Util::renderArrow(dataTODO.intersection, dataTODO.intersection + dataTODO.direction, 1.0f, 0.5f, P * V, colourPassThroughEffect);
		glEnable(GL_DEPTH_TEST);
	}

	glDisable(GL_DEPTH_TEST);
	renderer::bind(colourPassThroughEffect);

	glPointSize(6.0f);
	// Set MVP matrix uniform
	glUniformMatrix4fv(
		colourPassThroughEffect.get_uniform_location("MVP"), // Location of uniform
		1, // Number of values - 1 mat4
		GL_FALSE, // Transpose the matrix?
		value_ptr(P * V)); // Pointer to matrix data
	glUniform4fv(colourPassThroughEffect.get_uniform_location("colour"), 1, value_ptr(vec4(0, 0, 1, 1)));
	glBegin(GL_POINTS);
	glVertex3f(rayTest.intersection.x, rayTest.intersection.y, rayTest.intersection.z);
	glEnd();
	glEnable(GL_DEPTH_TEST);

	// TODO IK
	glDisable(GL_DEPTH_TEST);
	renderer::bind(colourPassThroughEffect);
	glUniform4fv(colourPassThroughEffect.get_uniform_location("colour"), 1, value_ptr(vec4(1, 0, 0, 1)));
	updateIK(P, V);
	glEnable(GL_DEPTH_TEST);

	
	// Disable wireframe
	glPolygonMode(GL_FRONT, GL_FILL);
	
	// Apply post processing effects
	postProcessing();
	
	// Finish rendering this frame
	finishFrame();

	return true;
}

void postProcessing(){

	// Disable Depth Testing
	glDisable(GL_DEPTH_TEST);

	//------------------ Start post processing start ------------------------

	// Bind the front post processing framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, postProcessingFbo.get_buffer());

	// Bind the Screen Space Ambient Occlusion effect
	renderer::bind(postProcessingEffect);

	glUniformMatrix4fv(
		postProcessingEffect.get_uniform_location("MVP"), // Location of uniform
		1, // Number of values - 1 mat4
		GL_FALSE, // Transpose the matrix?
		value_ptr(orthoMVP)); // Pointer to matrix data

	// Bind the last frame buffer's texture - MOTIONBLUR
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, prevFbo.get_texture());
	glUniform1i(postProcessingEffect.get_uniform_location("prevTex"), 3);

	// Bind the lens flare dirt texture - LENSFLARE
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, texs["lensflare"].get_id());
	glUniform1i(postProcessingEffect.get_uniform_location("dirt"), 2);

	// Bind the frame buffer's texture
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, fbo.get_depth());
	glUniform1i(postProcessingEffect.get_uniform_location("depth"), 1);

	// Bind the frame buffer's texture
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, fbo.get_texture());
	glUniform1i(postProcessingEffect.get_uniform_location("tex"), 0);

	// Send the screen dimensions to the shader
	glUniform1f(postProcessingEffect.get_uniform_location("screenWidth"), (float)renderer::get_screen_width());
	glUniform1f(postProcessingEffect.get_uniform_location("screenHeight"), (float)renderer::get_screen_height());

	// Send the near and far values to the shader
	glUniform1f(postProcessingEffect.get_uniform_location("near"), MYNEAR);
	glUniform1f(postProcessingEffect.get_uniform_location("far"), MYFAR);

	// Set the number of ghosts and their dispersal - LENSFLARE
	glUniform1i(postProcessingEffect.get_uniform_location("ghostCount"), 6);
	glUniform1f(postProcessingEffect.get_uniform_location("ghostDispersal"), 0.15f);

	// Send the shader the vignette parameters (outer + inner radius and the intensity of the vignette) - VIGNETTE
	glUniform1f(postProcessingEffect.get_uniform_location("outerRadius"), vignetteOuterRadius);
	glUniform1f(postProcessingEffect.get_uniform_location("innerRadius"), vignetteInnerRadius);
	glUniform1f(postProcessingEffect.get_uniform_location("intensity"), vignetteIntensity);

	// Send the shader the degree of blurring (rings + samples per ring) - SSAO + DOF
	glUniform1i(postProcessingEffect.get_uniform_location("samples"), 3);
	glUniform1i(postProcessingEffect.get_uniform_location("rings"), 3);

	// Send the shader the ssao parameters - SSAO
	glUniform1f(postProcessingEffect.get_uniform_location("aoRadius"), 1.0f);
	glUniform1f(postProcessingEffect.get_uniform_location("lumInfluence"), ssaoLumInfluence);
	glUniform1f(postProcessingEffect.get_uniform_location("selfShadowReduc"), ssaoSelfShadowReduc);
	glUniform1f(postProcessingEffect.get_uniform_location("gaussDisplace"), ssaoGaussDisplace);

	// Pass toggles
	glUniform1i(postProcessingEffect.get_uniform_location("ssaoEnabled"), ssaoEnabled);
	glUniform1i(postProcessingEffect.get_uniform_location("glowEnabled"), glowEnabled);
	glUniform1i(postProcessingEffect.get_uniform_location("lensFlareEnabled"), lensFlareEnabled);
	glUniform1i(postProcessingEffect.get_uniform_location("motionBlurEnabled"), motionBlurEnabled);
	glUniform1i(postProcessingEffect.get_uniform_location("greyscaleEnabled"), greyscaleEnabled);
	glUniform1i(postProcessingEffect.get_uniform_location("vignetteEnabled"), vignetteEnabled);

	// Tell the shader whether to use noise - SSAO
	glUniform1i(postProcessingEffect.get_uniform_location("useNoise"), ssaoNoise);
	// Tell the shader whether to use colour + ssao or just ssao - SSAO
	glUniform1i(postProcessingEffect.get_uniform_location("renderOnlyAO"), ssaoOnly);

	// Render the sceen quad
	renderer::render(screenQuad);

	//------------------ Start post processing start ------------------------

	// Re-enable Depth Testing
	glEnable(GL_DEPTH_TEST);

}

void finishFrame(){

	glDisable(GL_DEPTH_TEST);
	
	// Set the current shader to the pass-through shader
	renderer::bind(passThroughEffect);
	
	glUniformMatrix4fv(
		passThroughEffect.get_uniform_location("MVP"), // Location of uniform
		1, // Number of values - 1 mat4
		GL_FALSE, // Transpose the matrix?
		value_ptr(orthoMVP)); // Pointer to matrix data


	// Render the finished frame to the previous frame fbo
	glBindTexture(GL_TEXTURE_2D, postProcessingFbo.get_texture());
	glUniform1i(passThroughEffect.get_uniform_location("tex"), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, prevFbo.get_buffer());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderer::render(screenQuad);

	// Render the finished frame to the screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	renderer::render(screenQuad);

	if (toggleDebugMenu){
		// Render the cornerCam onto the screen
		glBindTexture(GL_TEXTURE_2D, shadowFbo.get_depth());
		glUniform1i(passThroughEffect.get_uniform_location("tex"), 0);
		renderer::render(cornerCamQuad);
	}

	// Render the menu onto the screen
	if (toggleDebugMenu){
		renderer::bind(texs["menuon"], 2);
	}
	else{
		renderer::bind(texs["menuoff"], 2);
	}
	glUniform1i(passThroughEffect.get_uniform_location("tex"), 2);
	renderer::render(menuQuad);


	glUniform1i(passThroughEffect.get_uniform_location("tex"), 0);
	textRen.render("Hello World!");

	glEnable(GL_DEPTH_TEST);
}

// Compares two scene objects in terms of distance from their origin
bool orderByDistance(SceneObject* sObj1, SceneObject* sObj2){
	float dist1, dist2;

	dist1 = sqrt(pow((camPos.x - sObj1->get_transform_with_parent().position.x), 2)
		+ pow((camPos.y - sObj1->get_transform_with_parent().position.y), 2)
		+ pow((camPos.z - sObj1->get_transform_with_parent().position.z), 2));

	dist2 = sqrt(pow((camPos.x - sObj2->get_transform_with_parent().position.x), 2)
		+ pow((camPos.y - sObj2->get_transform_with_parent().position.y), 2)
		+ pow((camPos.z - sObj2->get_transform_with_parent().position.z), 2));

	return dist1 > dist2;
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