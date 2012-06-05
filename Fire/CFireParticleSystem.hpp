//
//  CFireParticleSystem.hpp
//  FireOnPaper
//
//  Created by Wang Zhongyu on 12-5-28.
//  Copyright (c) 2012年 Peking University. All rights reserved.
//

#ifndef FireOnPaper_CFireParticleSystem_hpp
#define FireOnPaper_CFireParticleSystem_hpp

#include "OpenGLESContext.hpp"
#include "CFireParticle.hpp"
#include "Parameters.hpp"

Vector3D NULL_VECTOR3D = Vector3D(0.0f, 0.0f, 0.0f);

void  CFireParticleSystem::SetEmitter(Vector3D pos,Vector3D dev)
{
	m_EmitterPosition=pos;
	m_CreationDeviation=dev;  
}

void  CFireParticleSystem::SetEmissionDirection(Vector3D direction, Vector3D dev)
{
	m_EmitDirection=direction;
	m_EmitDirectionDeviation=dev; 
}

void  CFireParticleSystem::SetAcceleration(Vector3D acc, float Min, float Max)
{
	m_AccelerationDirection=acc;
	m_fMinAcceleration=Min;
	m_fMaxAcceleration=Max;
}


void  CFireParticleSystem::SetCreationDieColor(Vector3D MinEmit, Vector3D MaxEmit,Vector3D MinDie, Vector3D MaxDie)
{
	m_MinEmitColor=MinEmit;
	m_MaxEmitColor=MaxEmit;
	m_MinDieColor=MinDie;
	m_MaxDieColor=MaxDie;
}

void  CFireParticleSystem::SetAlphaValues(float MinEmit, float MaxEmit, float MinDie, float MaxDie)
{
	m_fMinEmitAlpha=MinEmit;
	m_fMaxEmitAlpha=MaxEmit;
	m_fMinDieAlpha=MinDie;
	m_fMaxDieAlpha=MaxDie;
}

void  CFireParticleSystem::SetSpinSpeed(float Min, float Max)
{
	m_fMinEmitSpinSpeed=Min;
	m_fMaxEmitSpinSpeed=Max;
}


void  CFireParticleSystem::SetSizeValues(float EmitMin, float EmitMax, float DieMin, float DieMax)
{
	m_fMinEmitSize=EmitMin;
	m_fMaxEmitSize=EmitMax;
	m_fMinDieSize=DieMin;
	m_fMaxDieSize=DieMax;
}

CFireParticleSystem::CFireParticleSystem()
{
	m_EmitterPosition=NULL_VECTOR3D;
	m_CreationDeviation=NULL_VECTOR3D;  
	
	m_EmitDirection=NULL_VECTOR3D;
	m_EmitDirectionDeviation=NULL_VECTOR3D;
	
	/**********************************
	 这两对重要参数将直接在类初始化
	 **********************************/
	m_fMinEmitSpeed=m_fMaxEmitSpeed=0.0f;
	m_fMinDieAge=m_fMaxDieAge=0.0f;
	/**********************************/
	
	m_AccelerationDirection=NULL_VECTOR3D;
	m_fMinAcceleration=m_fMaxAcceleration=0.0f;
	
	m_fMinEmitAlpha=m_fMaxEmitAlpha=m_fMinDieAlpha=m_fMaxDieAlpha=0.0f;
	
	m_MinEmitColor=NULL_VECTOR3D;
	m_MaxEmitColor=NULL_VECTOR3D;
	m_MinDieColor=NULL_VECTOR3D;
	m_MaxDieColor=NULL_VECTOR3D;
	
	m_fMinEmitSpinSpeed=m_fMaxEmitSpinSpeed=0.0f;
	//匀变速圆周运动
	m_fMinSpinAcceleration=m_fMaxSpinAcceleration=0.0f;
	
	m_fMinEmitSize=m_fMaxEmitSize=m_fMinDieSize=m_fMaxDieSize=1.0f;
	
	m_bRecreateWhenDied=true;  
	
	m_bUseTexture=true;
	
	m_iMaxParticles=m_iParticlesInUse=0;
	
	m_bRecreateWhenDied = false;
}

#endif
