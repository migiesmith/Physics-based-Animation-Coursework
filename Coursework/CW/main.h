/*
	Grant Smith (40111906)

	The main class handles the loading in of models and textures, and the updating and rendering of the SceneObjects

*/

//TODO

#include "SphereCollider.h"
#include "CubeCollider.h"

//TODO

#pragma once

#include <graphics_framework.h>
#include "renderer.h"
#include "SceneObject.h"
#include "FrameBuffer.h"

class SceneObject;

using namespace std;
using namespace graphics_framework;


float NEAR = 2.414f;
float FAR = 3500.0f;

// Debug menu toggle
bool toggleDebugMenu = false;

/*
	Post Processing Effects toggles
*/
bool motionBlurEnabled = true;
bool glowEnabled = true;
bool ssaoEnabled = true;
bool greyscaleEnabled = false;
bool vignetteEnabled = true;
bool dofEnabled = true;
bool lensFlareEnabled = true;

static effect mainEffect; // The main effect used
static effect passThroughEffect; // The effect used for the skybox
static effect fboEffect; // The effect used for the skybox
static effect spotLightEffect; // The effect used for the spot light
static effect grassEffect; // The effect used for the grass
static effect grassSpotEffect; // The effect used for the grass

static effect motionBlurEffect; // The effect used for the motion blur post process
static effect glowEffect; // The effect used for the glow post process
static effect ssaoEffect; // The effect used for the ssao post process
static effect greyscaleEffect; // The effect used for the greyscale post process
static effect vignetteEffect; // The effect used for the vignette post process
static effect dofEffect; // The effect used for the depth of field post process
static effect depthEffect; // The effect used for the ssao post process
static effect lensFlareEffect; // The effect used for blurring textures

// Dof values
float dofFocalLength = 300.0f; // The focal length of the camera
float dofFocalSize = 10.0f; // The focal size (size in depth)
bool dofDebug = false;

// SSAO values
bool ssaoNoise = true;
bool ssaoOnly = false;
float ssaoLumInfluence = 0.8f;
float ssaoSelfShadowReduc = 0.4f;
float ssaoGaussDisplace = 0.4f;

// Vignette values
float vignetteOuterRadius = 0.65f;
float vignetteInnerRadius = 0.4f;
float vignetteIntensity = 0.3f;

FrameBuffer frontPostProcessingFbo;
FrameBuffer backPostProcessingFbo;
FrameBuffer fbo;
FrameBuffer prevFbo;
FrameBuffer shadowFbo;

static mesh screenQuad; // Quad used for post processing
static mesh cornerCamQuad; // Quad used for rendering a texture to the corner of the screen
static mesh menuQuad; // Quad used for rendering the menu

// Enum for current camera values
enum Camera
{
	Chase, Target, Free
};

// Enum for menu buttons
enum Buttons
{
	Vignette, Motion_Blur, Glow, Grey_Scale, Depth_of_Field, DoF_Debug,
	Free_Cam, SSAO_Only, SSAO, WireFrame, Lens_Flare, Spot_Light, Target_Cam, Chase_Cam
};

mat4 orthoMVP; // MVP used for postprocessing

bool movingCam = false;

int currentCamera = 1; // Default to Target Camera
short targetCamDirection = -1; // The direction that the targetCam rotates

static material grassMat; // The material for the grass


texture loadTexture(string textureName, bool mipmaps, bool antisoptrics);
texture loadTexture(string textureName);
geometry loadModel(string modelName);

void copyBackFBO();
void postProcessing();
void finishFrame();
bool orderByDistance(SceneObject* sObj1, SceneObject* sObj2);
void initSceneObjects();
void initScreenQuads();
void initShaders();
void initCameras();
