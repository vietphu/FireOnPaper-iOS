//
//  FireOnPaperEngine.hpp
//  FireOnPaper
//
//  Created by Wang Zhongyu on 12-6-5.
//  Copyright (c) 2012年 Peking University. All rights reserved.
//

#ifndef FireOnPaper_FireOnPaperEngine_hpp
#define FireOnPaper_FireOnPaperEngine_hpp

#include "OpenGLESContext.hpp"
#include "FireOnPaperEnv.hpp"
#include "CFireParticle.hpp"
#include "Parameters.hpp"
#include "PaperUnit.hpp"
#include "SpaceUnit.hpp"
#include "Paper.hpp"

#define FIRE_NUM 3000
#define FIRE_SYSTEM_NUM 1

#define MODE_DEFENCE 0
#define MODE_FIRE 1
#define MODE_SHOW3D 2

class FireOnPaperEngine
{
private:
	// texture IDs
	GLuint * textureImageID;
	// gles-bc
	OpenGLES::OpenGLESContext * gl;
	// Environment
    Env * env;
	// play mode
	int play_mode;
	// Paper
    Paper * paper;
	// Fire
	CFireParticleSystem fire_system[FIRE_SYSTEM_NUM];
    
	// boolean for stopping rendering or not
    bool mIsStopRendering;
	
	// Change view point attributes
	double m_angle;
	double m_dx;
	double m_dy;
	double m_dz;

public:
	FireOnPaperEngine();
    // initialization after new FireOnPaperEngine() or reinit
	void Initialize(int width, int height, OpenGLES::OpenGLESContext * _gl, GLuint * _textureImageID, int _play_mode);
	// Change view point
	void ChangeViewPoint(double angle, double x, double y, double z);
	// Render stage (Drawing of particles)
	void Render();
    // Update stage (Simulation of particles)
    void UpdateAnimation(float timeStep);
	// Show rotating influence with accelerometer
	void OnRotateWithAccelerometer(double x, double y, double z);
	// Show disturbing influence with microphone
	void OnDisturbWithMicrophone(bool has_direction, bool from_left, double disturbing_coefficient);

	// Defence mode
	void OnTouchChangePaperMaterial(float _x_0_1, float _y_0_1);
    // Fire mode
	void OnTouchPaperToBurn(float _x_0_1, float _y_0_1);
    void SetStopRendering();
    ~FireOnPaperEngine();
};

#endif
