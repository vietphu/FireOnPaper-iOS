//
//  FireOnPaperEngine.cpp
//  FireOnPaper
//
//  Created by Wang Zhongyu on 12-6-5.
//  Copyright (c) 2012年 Peking University. All rights reserved.
//

#include <iostream>
#include "FireOnPaperEngine.hpp"

FireOnPaperEngine::FireOnPaperEngine()
{
	
}

void FireOnPaperEngine::Initialize(int width, int height, OpenGLES::OpenGLESContext * _gl, GLuint * _textureImageID, bool isDefenceMode)
{
	gl = _gl;
	textureImageID = _textureImageID;
    
    env = new Env();
    
    paper = new Paper();
    paper->Initialize(gl, textureImageID, env);
    bool logicPaper[PAPER_LOGIC_HEIGHT][PAPER_LOGIC_WIDTH];
    for ( int y=0; y<PAPER_LOGIC_HEIGHT; y++)
        for ( int x=0; x<PAPER_LOGIC_WIDTH; x++)
            logicPaper[y][x] = true;
    paper->SetPaperLogicShape(logicPaper, 0, 0);
    paper->SetPaperWorldSize(PAPER_WIDTH, PAPER_HEIGHT, PAPER_LEFT_TOP_X, PAPER_LEFT_TOP_Y);
    if (isDefenceMode) paper->GivePaperFirstFire();
	
	for (int i = 0; i < FIRE_SYSTEM_NUM; i++) fire_system[i].Initialize(gl, textureImageID, env, FIRE_NUM);
	
	gl->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	gl->glShadeModel(GL_SMOOTH);
	gl->glEnable(GL_DEPTH_TEST);
	gl->glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    gl->glEnableClientState(GL_VERTEX_ARRAY);
    
    mIsStopRendering = false;
}

void FireOnPaperEngine::Render()
{	
    if (mIsStopRendering)
        return;
    
	gl->glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	gl->glMatrixMode(GL_PROJECTION);
	gl->glLoadIdentity();
    gl->glOrthof(-SCREEN_WIDTH/2, SCREEN_WIDTH/2, -SCREEN_HEIGHT/2, SCREEN_HEIGHT/2, -10.0f, 10.0f);
    
    paper->Render();
	
	for (int i = 0; i < FIRE_SYSTEM_NUM; i++) fire_system[i].Render();
}

void FireOnPaperEngine::UpdateAnimation(float timeStep)
{
    if (mIsStopRendering)
        return;
    
    paper->DoPaperCreateHeat();
    paper->DoSpaceToSpaceHeatSpread();
	
    //for (int i = 0; i < FIRE_SYSTEM_NUM; i++) fire_system[i].OnDisturbWithMicrophone(false, true, 4.0);
	for (int i = 0; i < FIRE_SYSTEM_NUM; i++) fire_system[i].UpdateAnimation(timeStep);
    
    paper->DoFireToSpaceHeatSpread();
    paper->DoRefreshPaperStatus();
}

void FireOnPaperEngine::OnRotateWithAccelerometer(double x, double y, double z)
{
	paper->OnRotateWithAccelerometer(x, y, z);
	
	for (int i = 0; i < FIRE_SYSTEM_NUM; i++) fire_system[i].OnRotateWithAccelerometer(x, y, z);	
}

void FireOnPaperEngine::OnDisturbWithMicrophone(bool has_direction, bool from_left, double disturbing_coefficient)
{
	for (int i = 0; i < FIRE_SYSTEM_NUM; i++) fire_system[i].OnDisturbWithMicrophone(has_direction, from_left, disturbing_coefficient);
}

void FireOnPaperEngine::OnTouchChangePaperMaterial(float _x_0_1, float _y_0_1)
{
    paper->TouchChangePaperMaterial((int)(_x_0_1 * SPACE_LOGIC_WIDTH), (int)(_y_0_1 * SPACE_LOGIC_HEIGHT));
    paper->TouchChangePaperMaterial((int)(_x_0_1 * SPACE_LOGIC_WIDTH-1), (int)(_y_0_1 * SPACE_LOGIC_HEIGHT));
    paper->TouchChangePaperMaterial((int)(_x_0_1 * SPACE_LOGIC_WIDTH), (int)(_y_0_1 * SPACE_LOGIC_HEIGHT-1));
}

void FireOnPaperEngine::OnTouchPaperToBurn(float _x_0_1, float _y_0_1)
{
    paper->TouchPaperToBurn((int)(_x_0_1 * SPACE_LOGIC_WIDTH), (int)(_y_0_1 * SPACE_LOGIC_HEIGHT));
}

void FireOnPaperEngine::SetStopRendering()
{
    mIsStopRendering = true;
}

FireOnPaperEngine::~FireOnPaperEngine()
{
    delete paper;
	
    delete env;
}