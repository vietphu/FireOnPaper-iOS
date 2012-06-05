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

class FireOnPaperEngine
{
private:
	GLuint * textureImageID;
	OpenGLES::OpenGLESContext * gl;
    Env * env;
    Paper * paper;
	CFireParticleSystem fire_system[FIRE_SYSTEM_NUM];
	
public:
	FireOnPaperEngine();
    void Initialize(int width, int height, OpenGLES::OpenGLESContext * _gl, GLuint * _textureImageID);
    void Render();
    void UpdateAnimation(float timeStep);
	void OnRotateWithAccelerometer(double x, double y, double z);
	void OnDisturbWithMicrophone(bool has_direction, bool from_left, double disturbing_coefficient);
    void OnTouchChangePaperMaterial(float _x_0_1, float _y_0_1);
    ~FireOnPaperEngine();
};

#endif
