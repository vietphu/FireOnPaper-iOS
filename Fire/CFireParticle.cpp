//
//  CFireParticle.cpp
//  FireOnPaper
//
//  Created by Wang Zhongyu on 12-5-28.
//  Copyright (c) 2012年 Peking University. All rights reserved.
//

#include <iostream>
#include "CFireParticle.hpp"
#include "Parameters.hpp"
#include "FireOnPaperEnv.hpp"

CFireParticle::CFireParticle()
{
	m_fAge = 0.0f;
	m_fDieAge = 3.1415926f;
	m_bIsAlive = false;
}

// initialization after new CFireParticle() or reinit
void CFireParticle::Initialize(CFireParticleSystem *ParentSystem, OpenGLES::OpenGLESContext *_gl, int x, int y)
{
	gl = _gl;
	
	// Appearance Adjust
	// 如果初始化的时候之前的粒子未死亡
	// 那么我们不会真正初始化它
	// 而是将它移动到新增火焰点遮住纸片的缺口
	// 然后继续按照原有粒子运动模式运动
	if (m_fAge < m_fDieAge && m_fAge >= 0.0001f) {
		m_Position = Vector3D(-logicSpaceX2ScreenX(x), -logicSpaceY2ScreenY(y), 0);
		return ;
	}
	// Appearance Adjust
	
	// age is 0
	m_fAge = 0.0f;
	
	// calculate age of dying
	m_fDieAge = ParentSystem->m_fMinDieAge + 
	((ParentSystem->m_fMaxDieAge - ParentSystem->m_fMinDieAge)*RANDOM_FLOAT);
	if (m_fDieAge == 0.0f) m_fDieAge=2.0;  // give age 2.0 if result is 0
	
	// center of emitter
	m_Position = Vector3D(-logicSpaceX2ScreenX(x), -logicSpaceY2ScreenY(y), 0);
	// calculate a spherical emitter of with deviation
	m_Position.x += ParentSystem->m_CreationDeviation.x*(RANDOM_FLOAT*2.0f-1.0f);
	m_Position.y += ParentSystem->m_CreationDeviation.y*(RANDOM_FLOAT*2.0f-1.0f);
	m_Position.z += ParentSystem->m_CreationDeviation.z*(RANDOM_FLOAT*2.0f-1.0f);
	
	// velocity of emitting calculated by direction of emitting, deviation of emitting direction, min and max speed of emitting
	m_Velocity.x = ParentSystem->m_EmitDirection.x + ParentSystem->m_EmitDirectionDeviation.x*(RANDOM_FLOAT*2.0f-1.0f);
	m_Velocity.y = ParentSystem->m_EmitDirection.y + ParentSystem->m_EmitDirectionDeviation.y*(RANDOM_FLOAT*2.0f-1.0f);
	m_Velocity.z = ParentSystem->m_EmitDirection.z + ParentSystem->m_EmitDirectionDeviation.z*(RANDOM_FLOAT*2.0f-1.0f);
	m_Velocity = m_Velocity * ((ParentSystem->m_fMinEmitSpeed + 
								(ParentSystem->m_fMaxEmitSpeed - ParentSystem->m_fMinEmitSpeed)*RANDOM_FLOAT));
	
	// accleration of emitting calculated by acceleration direction of emitting, min and max acceleration value
	m_Acceleration = ParentSystem->m_AccelerationDirection* 
	(ParentSystem->m_fMinAcceleration + 
	 (ParentSystem->m_fMaxAcceleration-ParentSystem->m_fMinAcceleration)*RANDOM_FLOAT);
	
	// color of emitting calculated by min and max color(RGB) of emitting
	m_Color = ParentSystem->m_MinEmitColor + 
	((ParentSystem->m_MaxEmitColor-ParentSystem->m_MinEmitColor) * RANDOM_FLOAT);
	// color change rate calculated by min and max color(RGB) of emitting and dying
	Vector3D EndColor = ParentSystem->m_MinDieColor + 
	((ParentSystem->m_MaxDieColor-ParentSystem->m_MinDieColor) * RANDOM_FLOAT);
	m_ColorChange = (EndColor-m_Color) / m_fDieAge;
	
	// alpha of emitting calculated by min and max color(RGB) of emitting
	m_fAlpha = ParentSystem->m_fMinEmitAlpha 
	+ ((ParentSystem->m_fMaxEmitAlpha - ParentSystem->m_fMinEmitAlpha) * RANDOM_FLOAT);
	float fEndAlpha = ParentSystem->m_fMinDieAlpha 
	+ ((ParentSystem->m_fMaxDieAlpha - ParentSystem->m_fMinDieAlpha) * RANDOM_FLOAT);
	// alpha change rate calculated by min and max color(RGB) of emitting and dying
	m_fAlphaChange = (fEndAlpha - m_fAlpha) / m_fDieAge;
	
	// size of emitting calculated by min and max color(RGB) of emitting
	m_fSize = ParentSystem->m_fMinEmitSize +
	((ParentSystem->m_fMaxEmitSize - ParentSystem->m_fMinEmitSize) * RANDOM_FLOAT);
	float fEndSize = ParentSystem->m_fMinDieSize +
	((ParentSystem->m_fMaxDieSize - ParentSystem->m_fMinDieSize) * RANDOM_FLOAT);
	// size change rate calculated by min and max color(RGB) of emitting and dying
	m_fSizeChange = (fEndSize - m_fSize) / m_fDieAge;
	
	// spin angle starts with 0
	m_fSpinAngle = 0.0f;
	// spin speed calculated by min and max spin speed of emitting
	m_fSpinSpeed = ParentSystem->m_fMinEmitSpinSpeed +
	((ParentSystem->m_fMaxEmitSpinSpeed - ParentSystem->m_fMinEmitSpinSpeed) * RANDOM_FLOAT);
	// spin acceleration calculated by min and max spin acceleration of emitting
	m_fSpinAcceleration = ParentSystem->m_fMinSpinAcceleration +
	((ParentSystem->m_fMaxSpinAcceleration - ParentSystem->m_fMinSpinAcceleration) * RANDOM_FLOAT);
	
	// active the particle
	m_bIsAlive = true;
	// tell the parent particle system
	ParentSystem->m_iParticlesInUse++;
	m_ParentSystem = ParentSystem;
}

// Update stage (Simulation of particle)
void CFireParticle::Update(float timePassed, int x, int y)
{
	// age increases
	m_fAge += timePassed;
	
	// if it died
	if (m_fAge >= m_fDieAge) 
	{
		m_fAge = 0.0f;
		m_bIsAlive = false;
			
		m_ParentSystem->m_iParticlesInUse--;
		if (m_ParentSystem->m_bRecreateWhenDied)
			Initialize(m_ParentSystem, gl, x, y);
		else
			return;
	}
	
	// change alpha and color
	m_fAlpha += m_fAlphaChange*timePassed;
	m_Color = m_Color + m_ColorChange*timePassed;
	
	// change velocity: v += a*dt
	m_Velocity = m_Velocity + m_Acceleration*timePassed;
	
	// change position: s += v*dt
	m_Position = m_Position + (m_Velocity*timePassed);
	
	// if uses texture
	if(m_ParentSystem->m_bUseTexture==true)
	{
		// v += a*dt s += v*dt
		m_fSpinSpeed += m_fSpinAcceleration*timePassed;
		m_fSpinAngle  += m_fSpinSpeed*timePassed;
		
		// if spin angle > 2*PI
		if(m_fSpinAngle>2*GLT_PI)
			m_fSpinAngle = m_fSpinAngle - 2*GLT_PI;
	}
	else 
		;
	
	// change size
	m_fSize  += m_fSizeChange *timePassed;
}

// Render stage (Drawing of particle)
void CFireParticle::Render()
{
	// if not use texture
	if (!m_ParentSystem->m_bUseTexture) 
	{
		
		gl->glPointSize(m_fSize);
		
		float color[4];
		color[0] = m_Color.x;
		color[1] = m_Color.y;
		color[2] = m_Color.z;
		color[3] = m_fAlpha;
		
		// draw
		gl->glColorPointer(	4,   //(rgba)
						   GL_FLOAT,
						   0,
						   &color[0]);
		
		gl->glVertexPointer(	3,   //vertex (x,y,z)
							GL_FLOAT,
							0,
							&m_Position.x);
		
		gl->glDrawArrays(GL_POINTS,
						 0,
						 1);
		
	}
	else
	{
		GLfloat colorer[4][4];
		for(int i=0;i<4;i++)
		{
			colorer[i][0] = m_Color.x;
			colorer[i][1] = m_Color.y;
			colorer[i][2] = m_Color.z;
			colorer[i][3] = m_fAlpha;
			if (m_fAge < m_fDieAge * 1.0f / 2.0f) {
				// yellow at beginning
				colorer[i][0] = 1.f;
				colorer[i][1] = 0.98f;
				colorer[i][2] = 0.4f;
				colorer[i][3] = m_fAlpha;
			}
			if (m_fAge >= m_fDieAge * 1.0f / 2.0f) {
				// red at ending
				colorer[i][0] = 0.875f;
				colorer[i][1] = 0.302f;
				colorer[i][2] = 0.196f;
				colorer[i][3] = m_fAlpha;
			}
		}
		
		// Coordinate of texture
		GLshort coord[4][2]={0,0,1,0,1,1,0,1};
		
		float  lengthOF=(float)(m_fSize *1.414*0.707);
		float  xOF=(float)cos(m_fSpinAngle);
		float  yOF=(float)sin(m_fSpinAngle);
		
		// Coordinate of rectangle vertex
		GLfloat  vertex[4][3];
		
		vertex[0][0]=lengthOF*(yOF-xOF)-m_Position.x;
		vertex[0][1]=lengthOF*(-xOF-yOF)-m_Position.y;
		vertex[0][2]=m_Position.z;
		
		vertex[1][0]=lengthOF*(xOF+yOF)-m_Position.x;
		vertex[1][1]=lengthOF*(yOF-xOF)-m_Position.y;
		vertex[1][2]=m_Position.z;
		
		vertex[2][0]=lengthOF*(xOF-yOF)-m_Position.x;
		vertex[2][1]=lengthOF*(xOF+yOF)-m_Position.y;
		vertex[2][2]=m_Position.z;
		
		vertex[3][0]=lengthOF*(-xOF-yOF)-m_Position.x;
		vertex[3][1]=lengthOF*(xOF-yOF)-m_Position.y;
		vertex[3][2]=m_Position.z;
		
		// draw
		gl->glColorPointer(4,GL_FLOAT,0,&colorer[0][0]);
		
		gl->glTexCoordPointer(
							  2,
							  GL_SHORT,
							  0,
							  &coord[0][0]
							  );
		
		gl->glVertexPointer(	3,   //vertex (x,y,z)
							GL_FLOAT,
							0,
							&vertex[0][0]);
		
		gl->glDrawArrays(GL_TRIANGLE_FAN,
						 0,
						 4);
		
	}
}

// Deactive particle
void CFireParticle::Deactive()
{
	// deactive as the situation when age > dieAge
	m_fAge = 0.0f;
	m_bIsAlive = false;
	m_ParentSystem->m_iParticlesInUse--;
	
	return;
}

// Get position of particle
Vector3D CFireParticle::GetPosition()
{
	return -m_Position;
}

// Get fire power of particle
float CFireParticle::GetPower()
{
	// 1 - 4 * (relative age - 0.5)^2
	float relativeAge = (m_fDieAge - m_fAge) / m_fDieAge;
	return 1-(relativeAge-1.0f/2.0f)*(relativeAge-1.0f/2.0f)*4.0;
	//return relativeAge;
}

// Show disturbing influence with microphone
void CFireParticle::OnDisturbWithMicrophone(double disturbing_coefficient_from_left, float timeStep)
{
	// change velocity to show
	m_Velocity = m_Velocity + Vector3D(m_Acceleration.y, -m_Acceleration.x, m_Acceleration.z)*disturbing_coefficient_from_left*timeStep;
	//m_Position = m_Position + (m_Velocity*timeStep);
}