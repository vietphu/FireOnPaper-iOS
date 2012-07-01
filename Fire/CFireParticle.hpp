//
//  CFireParticle.hpp
//  FireOnPaper
//
//  Created by Wang Zhongyu on 12-5-28.
//  Copyright (c) 2012年 Peking University. All rights reserved.
//

#ifndef FireOnPaper_CFireParticle_hpp
#define FireOnPaper_CFireParticle_hpp

#include "OpenGLESContext.hpp"
#include "Vector3D.hpp"
#include "FireOnPaperEnv.hpp"

// RANDOM_FLOAT 0.0~1.0
#define RANDOM_FLOAT (((float)rand())/RAND_MAX)
#define GLT_PI 3.14159265358979323846

class CFireParticleSystem;

class CFireParticle
{
private:
	
	// gles-bc
    OpenGLES::OpenGLESContext *gl;
	
	// position
	Vector3D m_Position;
	// velocity
	Vector3D m_Velocity;
	// acceleration
	Vector3D m_Acceleration;
	
	// alpha and alpha change rate
	float m_fAlpha;
	float m_fAlphaChange; 
	
	// color and color change rate
	Vector3D m_Color;   
	Vector3D m_ColorChange; 
	
	// age when dying and age now
	float m_fDieAge;
	float m_fAge;	
	
	/*--------------------------*
	 *  m_bUseTexture=TRUE		*
	 *  Use spin of rectangle	*
	 *--------------------------*/
	// spin angle now, spin speed and spin acceleration
	float m_fSpinAngle; 
	float m_fSpinSpeed;
	float m_fSpinAcceleration;
	
	/*--------------------------*
	 *  m_bUseTexture=FALSE		*
	 *  Use point to simulate	*
	 *--------------------------*/
	// point size and point size change rate
	float m_fSize;
	float m_fSizeChange;	
	
	// parent particle system
	CFireParticleSystem * m_ParentSystem;
	
public:
	CFireParticle();
	// is particle alive
	bool m_bIsAlive;
	// initialization after new CFireParticle() or reinit
	void Initialize(CFireParticleSystem * ParentSystem, OpenGLES::OpenGLESContext *_gl, int x, int y);
	
	// Update stage (Simulation of particle)
	void Update(float timePassed, int x, int y);
	// Render stage (Drawing of particle)
	void Render();
	// Deactive particle
	void Deactive();
	
	// Get position of particle
	Vector3D GetPosition();
	// Get fire power of particle
	float GetPower();
	// Show disturbing influence with microphone
	void OnDisturbWithMicrophone(double disturbing_coefficient_from_left, float timeStep);
};

class CFireParticleSystem
{
private:
	
	// gles-bc
	OpenGLES::OpenGLESContext * gl;
	// texture IDs
	GLuint * textureImageID;
	// Environment
	Env * env;
	// vertex buffer
	GLuint m_vertexBuffer;
	
	// number of particles
	int point_number;
	// fire particles
	CFireParticle * fire_particles;
	// position of particles
	int * particle_x, * particle_y;
	// last particle initialized
	int last_particle_initialized;
	
	// disturbing influence direction
	bool has_direction, from_left;
	// disturbing influence coefficient
	double disturbing_coefficient;
	// singal for disturbing influence direction (if no specific direction)
	int signal;
	// disturbing influence direction change cycle (if no specific direction)
	double change_cycle;
	
	// total elapsed time
	double totalTime;
	
public:
	/*--------------------------*
	 *  attributes for emitter	*
	 *--------------------------*/
	// emitter position
	Vector3D m_EmitterPosition;
	// deviation of creation position
	Vector3D m_CreationDeviation;  
	
	// direction of emitting
	Vector3D m_EmitDirection;
	// deviation of emitting direction
	Vector3D m_EmitDirectionDeviation; 
	// min and max speed of emitting
	float m_fMinEmitSpeed;
	float m_fMaxEmitSpeed;
	
	// acceleration direction of emitting
	Vector3D m_AccelerationDirection;
	// min and max acceleration value
	float m_fMinAcceleration;
	float m_fMaxAcceleration;
	
	// min and max alpha of emitting or dying
	float m_fMinEmitAlpha;
	float m_fMaxEmitAlpha;
	float m_fMinDieAlpha;
	float m_fMaxDieAlpha;
	// min and max color(RGB) of emitting or dying
	Vector3D m_MinEmitColor;
	Vector3D m_MaxEmitColor;
	Vector3D m_MinDieColor;
	Vector3D m_MaxDieColor;	
	// min and max age of dying
	float m_fMinDieAge;
	float m_fMaxDieAge;
	
	/*--------------------------*
	 *  m_bUseTexture=TRUE		*
	 *  Use spin of rectangle	*
	 *--------------------------*/
	// min and max spin speed of emitting
	float m_fMinEmitSpinSpeed;
	float m_fMaxEmitSpinSpeed;
	// min and max spin acceleration of emitting
	float m_fMinSpinAcceleration;
	float m_fMaxSpinAcceleration;
	
	/*--------------------------*
	 *  m_bUseTexture=FALSE		*
	 *  Use point to simulate	*
	 *--------------------------*/
	// min and max point size of emitting
	float m_fMinEmitSize;
	float m_fMaxEmitSize;
	float m_fMinDieSize;
	float m_fMaxDieSize;
	
	/*--------------------------*
	 *  Other attributes		*
	 *--------------------------*/	
	// whether particle recreates when it died
	bool m_bRecreateWhenDied;
	// whether use texture
	bool m_bUseTexture;
	// max particle number
	int m_iMaxParticles;
	// now particle number used
	int m_iParticlesInUse;
	
	CFireParticleSystem();
	
    // initialization after new CFireParticle() or reinit
	void Initialize(OpenGLES::OpenGLESContext * _gl, GLuint * _textureImageID, Env * _env, int _point_number);
	
	// Render stage (Drawing of particles)
	void BeforeRender();
    void Render();
	void AfterRender();
	
	// Update stage (Simulation of particles)
    void UpdateAnimation(float timeStep);
	// Show rotating influence with accelerometer
	void OnRotateWithAccelerometer(double x, double y, double z);
	// Show disturbing influence with microphone
	void OnDisturbWithMicrophone(bool _has_direction, bool _from_left, double _disturbing_coefficient);
	
	// set attributes
	void SetEmitter (Vector3D pos,Vector3D dev);
	void SetEmissionDirection(Vector3D direction, Vector3D dev);
	void SetAcceleration (Vector3D acc, float Min, float Max);
	void SetCreationDieColor (Vector3D MinEmit, Vector3D MaxEmit,Vector3D MinDie, Vector3D MaxDie);
	void SetAlphaValues (float MinEmit, float MaxEmit, float MinDie, float MaxDie);
	void SetSpinSpeed (float Min, float Max);
	void SetSizeValues (float EmitMin, float EmitMax, float DieMin, float DieMax);
	
    ~CFireParticleSystem();
};

#endif
