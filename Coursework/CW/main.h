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


float MYNEAR = 2.414f;
float MYFAR = 3500.0f;

float accumDeltaTime = 0.0f;
float PHYSICS_TIME_STEP = 0.01f;

// Debug menu toggle
bool toggleDebugMenu = false;
bool toggleParticles = true;
bool toggleIK = true;
bool toggleFullSPGrid = false;
bool toggleSPGrid = true;


static effect mainEffect; // The main effect used
static effect passThroughEffect; // The effect used for the skybox
static effect colourPassThroughEffect; // The effect used for the skybox

static mesh screenQuad; // Quad used for post processing
static mesh cornerCamQuad; // Quad used for rendering a texture to the corner of the screen
static mesh menuQuad; // Quad used for rendering the menu

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

bool movingCam = false;

int currentCamera = 1; // Default to Target Camera
short targetCamDirection = -1; // The direction that the targetCam rotates

TextRenderer* textRen;
GraphRenderer* graphRen;

void attemptToMakeAParticleEmitter();
void finishFrame();
void initSceneObjects();
void initScreenQuads();
void initParticleManager();
void initShaders();
void initCameras();
