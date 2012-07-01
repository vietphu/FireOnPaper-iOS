//
//  CFireParticleSystem.cpp
//  FireOnPaper
//
//  Created by Wang Zhongyu on 12-5-28.
//  Copyright (c) 2012年 Peking University. All rights reserved.
//

#include <iostream>
#include "CFireParticleSystem.hpp"

// initialization after new CFireParticle() or reinit
void CFireParticleSystem::Initialize(OpenGLES::OpenGLESContext *_gl, GLuint *_textureImageID, Env * _env, int _point_number)
{
	// record the parameters
	gl = _gl;
	textureImageID = _textureImageID;
	env = _env;
	point_number = _point_number;

	// initialize attributes of fire particle system
	fire_particles = new CFireParticle[point_number]();
	particle_x = new int[point_number];
	particle_y = new int[point_number];
	last_particle_initialized = 0;
	
	totalTime = 0.0f;
	signal = 1;
	change_cycle = RANDOM_FLOAT * 0.5f;
	
	m_fMinDieAge = 1.5f;
	m_fMaxDieAge = 0.5f;
	SetCreationDieColor(Vector3D(0.9f,0.8f,0.3f),Vector3D(0.9f,0.9f,0.4f),Vector3D(0.8f,0.724f,0.16f),Vector3D(0.81f,0.725f,0.17f));
	SetAlphaValues(1.0f,1.0f,0.0f,0.0f);
	SetSpinSpeed(-0.82*GLT_PI,0.82*GLT_PI);
#define SIZE_ONE 0.5f
	SetSizeValues(SIZE_ONE,2*SIZE_ONE,SIZE_ONE,2*SIZE_ONE);
	
	// set acceleration up at first
	SetAcceleration(Vector3D(0.0f,-10.0f,0.0f),1.3f,0.4f);
	
	m_fMaxEmitSpeed = 40.0f;
	m_fMinEmitSpeed = 10.0f;
	SetEmissionDirection(Vector3D(0.0f,0.0f,0.0f),Vector3D(0.0f,0.0f,0.0f));
	SetEmitter(Vector3D(0.0f,0.0f,0.0f), Vector3D(0.2f,0.2f,0.0f));
    
	// generate vertex buffer
    gl->glGenBuffers(1, &m_vertexBuffer);
}

// Render stage (Drawing of particles)
void CFireParticleSystem::BeforeRender()
{
	// OpenGLES state enable
	gl->glEnableClientState(GL_VERTEX_ARRAY);
	gl->glEnableClientState(GL_COLOR_ARRAY);
	gl->glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	gl->glEnable(GL_BLEND);
	gl->glDepthMask(GL_FALSE);
	gl->glEnable(GL_TEXTURE_2D);
	
	// bind vertex buffer and texture
	gl->glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	gl->glBindTexture(GL_TEXTURE_2D, textureImageID[1]);
}

void CFireParticleSystem::Render()
{
	BeforeRender();
	for (int i = 0; i < point_number; i++) {
		// if particle is active then render
		if (!fire_particles[i].m_bIsAlive) continue;
		fire_particles[i].Render();
	}
	AfterRender();
}

void CFireParticleSystem::AfterRender()
{
	// OpenGLES state disable
	gl->glDisable(GL_BLEND);
	gl->glDepthMask(GL_TRUE);
	gl->glDisable(GL_TEXTURE_2D);
    gl->glDisableClientState(GL_VERTEX_ARRAY);
	gl->glDisableClientState(GL_COLOR_ARRAY);
	gl->glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

// Update stage (Simulation of particles)
void CFireParticleSystem::UpdateAnimation(float timeStep)
{
	// get every position of fire generated from Environment
	for (int x = 0; x < SPACE_LOGIC_WIDTH; x++) {
		for (int y = 0; y < SPACE_LOGIC_HEIGHT; y++) {
			if (env->GetSpaceFire(x, y)) {
				// initialize the particle
				fire_particles[last_particle_initialized].Initialize(this, gl, x, y);
				particle_x[last_particle_initialized] = x;
				particle_y[last_particle_initialized] = y;
				last_particle_initialized = (last_particle_initialized+1) % point_number;
			}
		}
	}
	
	// reset the fire particle density matrix
	memset(env->mFireParticlesDensity, 0, sizeof(env->mFireParticlesDensity));
	for (int i = 0; i < point_number; i++) {
		if (!fire_particles[i].m_bIsAlive) continue;
		// if particle is active then update information
		fire_particles[i].Update(timeStep, particle_x[i], particle_y[i]);
		
		// record information into the fire particle density matrix
		Vector3D position = fire_particles[i].GetPosition();
        int logic_space_y = screenY2LogicSpaceY(position.y);
        int logic_space_x = screenX2LogicSpaceX(position.x);
        if (logic_space_x >= 0 && logic_space_x < SPACE_LOGIC_WIDTH && logic_space_y >= 0 && logic_space_y < SPACE_LOGIC_HEIGHT)
        {
            env->mFireParticlesDensity[logic_space_y][logic_space_x] += fire_particles[i].GetPower();
        }
	}
	
	// get total elapsed time
	totalTime += timeStep;
	// if it needs to change disturbing influence direction
	if (totalTime > change_cycle) {
		totalTime = 0.0f;
		signal = -signal;
		if (disturbing_coefficient > 2.0f) change_cycle = RANDOM_FLOAT / disturbing_coefficient;
		else change_cycle = RANDOM_FLOAT * 0.5f;
	}
	if (disturbing_coefficient < 0.25f) return ;
	// call particle's disturbing influence with microphone with corresponding direction and coefficient
	if (has_direction) {
		if (from_left) {
			for (int i = 0; i < point_number; i++) if (fire_particles[i].m_bIsAlive) fire_particles[i].OnDisturbWithMicrophone(RANDOM_FLOAT * disturbing_coefficient, timeStep);
		} else {
			for (int i = 0; i < point_number; i++) if (fire_particles[i].m_bIsAlive) fire_particles[i].OnDisturbWithMicrophone(-RANDOM_FLOAT * disturbing_coefficient, timeStep);
		}
		// make the influence less and less
		disturbing_coefficient = disturbing_coefficient /= 2;
	} else {
		for (int i = 0; i < point_number; i++) if (fire_particles[i].m_bIsAlive) fire_particles[i].OnDisturbWithMicrophone(signal * RANDOM_FLOAT * disturbing_coefficient, timeStep);
		// make the influence less and less
		disturbing_coefficient = disturbing_coefficient /= 2;
	}
}

// Show rotating influence with accelerometer
void CFireParticleSystem::OnRotateWithAccelerometer(double x, double y, double z)
{
	// set acceleration as function of accelerometer attributes
	SetAcceleration(Vector3D(x*10, y*10, z*10), 1.3f, 0.4f);
}

// Show disturbing influence with microphone
void CFireParticleSystem::OnDisturbWithMicrophone(bool _has_direction, bool _from_left, double _disturbing_coefficient)
{
	// initialize attributes related to disturbing influence
	has_direction = _has_direction;
	from_left = _from_left;
	disturbing_coefficient = _disturbing_coefficient * 2.0;
	totalTime = 0.0f;
	change_cycle = RANDOM_FLOAT / disturbing_coefficient;
}

CFireParticleSystem::~CFireParticleSystem()
{
	delete fire_particles;
	delete particle_x;
	delete particle_y;
}