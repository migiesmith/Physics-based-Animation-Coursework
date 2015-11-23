/*
	Grant Smith (40111906)

	The main class handles the loading in of models and textures, and the updating and rendering of the SceneObjects

*/

#pragma once

#include <graphics_framework.h>
#include "renderer.h"
#include "SceneObject.h"

#include "Link.h"
#include "SphereCollider.h"
#include "CubeCollider.h"
#include "LineCollider.h"
#include "GraphRenderer.h"
#include "ParticleEmitterManager.h"

#include "SPGrid.h"
#include "IKManager.h"

class SceneObject;

using namespace std;
using namespace graphics_framework;


float MYNEAR = 2.414f; // The near value of the camera (for the camera and ray casting)
float MYFAR = 3500.0f; // The far value of the camera (for the camera and ray casting)

float accumDeltaTime = 0.0f; // total time past when updating
float PHYSICS_TIME_STEP = 0.01f; // total time past for the physics engine

// Debug menu toggle
bool toggleDebugMenu = false; // toggles the debug menu
bool toggleParticles = true; // toggle the particle manager
bool toggleIK = true; // toggle the inverse kinematics manager
bool toggleFullSPGrid = false; // toggle the rendering of the full spatial partitioning grid
bool toggleSPGrid = true; // toggles the spatial partitioning grid (whether it should render at all)


static effect mainEffect; // The main effect used
static effect passThroughEffect; // The effect used for the skybox
static effect colourPassThroughEffect; // The effect used for the skybox

static mesh menuQuad; // Quad used for rendering the menu background

// Enum for current camera values
enum Camera
{
	Target, Free
};

// Enum for menu buttons
enum Buttons
{
	Particles, IK, FullSPGrid, SPGrid, ResetParticles, DoF_Debug,
	Free_Cam, SSAO_Only, SSAO, WireFrame, Lens_Flare, Spot_Light, Target_Cam, Chase_Cam
};

mat4 orthoMVP; // MVP used for postprocessing

int currentCamera = 1; // Default to Target Camera
short targetCamDirection = -1; // The direction that the targetCam rotates

// The text renderer used within the scene (for ui and debug info)
TextRenderer* textRen;
// The graph renderer used within the scene (for fps rendering)
GraphRenderer* graphRen;

// Sends a ray out, if it hits the plane, it creates an emitter at the intersection point
void attemptToMakeAParticleEmitter();
// Finish rendering the frame
void finishFrame();
// Initialise the scene objects
void initSceneObjects();
// Initialise the menu background quad
void initMenuQuad();
// Initialise the particle manager
void initParticleManager();
// Initialise the shaders
void initShaders();
// Initialise the cameras
void initCameras();
