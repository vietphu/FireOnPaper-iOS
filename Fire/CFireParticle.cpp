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

void CFireParticle::Initialize(CFireParticleSystem *ParentSystem, OpenGLES::OpenGLESContext *_gl, int x, int y)
{
	gl = _gl;
	//注意以下所有  (RANDOM_FLOAT*2.0f-1.0f)  的值为[-1,1] 创建正反
	
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
	
	//首先 年龄是0
	m_fAge = 0.0f;
	
	//没办法 它就是这样计算生命的
	m_fDieAge = ParentSystem->m_fMinDieAge + 
	((ParentSystem->m_fMaxDieAge - ParentSystem->m_fMinDieAge)*RANDOM_FLOAT);
	if (m_fDieAge == 0.0f) m_fDieAge=2.0;  //实在没有给它两秒吧
	
	//初始中心
	// m_Position = ParentSystem->m_EmitterPosition;
	m_Position = Vector3D(-logicSpaceX2ScreenX(x), -logicSpaceY2ScreenY(y), 0);
	//别忘了  还有偏移呢 这样我们能创建一个球形发射区
	//不然所有的粒子都是从一个点冒出来的  (-1,1)
	m_Position.x += ParentSystem->m_CreationDeviation.x*(RANDOM_FLOAT*2.0f-1.0f);
	m_Position.y += ParentSystem->m_CreationDeviation.y*(RANDOM_FLOAT*2.0f-1.0f);
	m_Position.z += ParentSystem->m_CreationDeviation.z*(RANDOM_FLOAT*2.0f-1.0f);
	
	
	//发射的方向   发射方向的偏移  和发射的最小最大速率  来计算随机的速度
	//首先 确定方位   然后确定大小
	m_Velocity.x = ParentSystem->m_EmitDirection.x + ParentSystem->m_EmitDirectionDeviation.x*(RANDOM_FLOAT*2.0f-1.0f);
	m_Velocity.y = ParentSystem->m_EmitDirection.y + ParentSystem->m_EmitDirectionDeviation.y*(RANDOM_FLOAT*2.0f-1.0f);
	m_Velocity.z = ParentSystem->m_EmitDirection.z + ParentSystem->m_EmitDirectionDeviation.z*(RANDOM_FLOAT*2.0f-1.0f);
	m_Velocity = m_Velocity * ((ParentSystem->m_fMinEmitSpeed + 
								(ParentSystem->m_fMaxEmitSpeed - ParentSystem->m_fMinEmitSpeed)*RANDOM_FLOAT));
	
	
	//同理 这里用 方向*大小  但是没有偏移  没必要吧
	m_Acceleration = ParentSystem->m_AccelerationDirection* 
	(ParentSystem->m_fMinAcceleration + 
	 (ParentSystem->m_fMaxAcceleration-ParentSystem->m_fMinAcceleration)*RANDOM_FLOAT);
	
	
	//发射颜色
	m_Color = ParentSystem->m_MinEmitColor + 
	((ParentSystem->m_MaxEmitColor-ParentSystem->m_MinEmitColor) * RANDOM_FLOAT);
	//首先我们计算出来死亡时候的颜色 （RGB）  除以生命 就是颜色变化率
	Vector3D EndColor = ParentSystem->m_MinDieColor + 
	((ParentSystem->m_MaxDieColor-ParentSystem->m_MinDieColor) * RANDOM_FLOAT);
	m_ColorChange = (EndColor-m_Color) / m_fDieAge;
	
	//同理颜色
	m_fAlpha = ParentSystem->m_fMinEmitAlpha 
	+ ((ParentSystem->m_fMaxEmitAlpha - ParentSystem->m_fMinEmitAlpha) * RANDOM_FLOAT);
	float fEndAlpha = ParentSystem->m_fMinDieAlpha 
	+ ((ParentSystem->m_fMaxDieAlpha - ParentSystem->m_fMinDieAlpha) * RANDOM_FLOAT);
	m_fAlphaChange = (fEndAlpha - m_fAlpha) / m_fDieAge;
	
	//同理颜色
	m_fSize = ParentSystem->m_fMinEmitSize +
	((ParentSystem->m_fMaxEmitSize - ParentSystem->m_fMinEmitSize) * RANDOM_FLOAT);
	float fEndSize = ParentSystem->m_fMinDieSize +
	((ParentSystem->m_fMaxDieSize - ParentSystem->m_fMinDieSize) * RANDOM_FLOAT);
	m_fSizeChange = (fEndSize - m_fSize) / m_fDieAge;
	
	//纹理旋转角度初始为0 我们用弧度计算
	m_fSpinAngle = 0.0f;
	//角速度  和   角加速度   a=a0+wt;
	m_fSpinSpeed = ParentSystem->m_fMinEmitSpinSpeed +
	((ParentSystem->m_fMaxEmitSpinSpeed - ParentSystem->m_fMinEmitSpinSpeed) * RANDOM_FLOAT);
	m_fSpinAcceleration = ParentSystem->m_fMinSpinAcceleration +
	((ParentSystem->m_fMaxSpinAcceleration - ParentSystem->m_fMinSpinAcceleration) * RANDOM_FLOAT);
	
	//好了  可以激活了   保留父类   重生时候继续使用
	m_bIsAlive = true;
	ParentSystem->m_iParticlesInUse++;
	
	m_ParentSystem = ParentSystem;
}

void CFireParticle::Update(float timePassed, int x, int y)
{
	//首先的年龄长大了
	m_fAge += timePassed;
	
	//如果都要死了
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
	
	//改变所有应该改变的
	m_fAlpha += m_fAlphaChange*timePassed;
	m_Color = m_Color + m_ColorChange*timePassed;
	
	// V= V0+at
	m_Velocity = m_Velocity + m_Acceleration*timePassed;
	
	//s=vt+1/2a*t*t  应该用这个对吗  是的  但是相信我  这里用这个就够了  方便计算
	//这么大量的数据  性能重要嘛   s+=vt;  注意哦  我们可没有重载+=
	m_Position = m_Position + (m_Velocity*timePassed);
	
	
	//下面的应该注意下    记得你使用的是三角形纹理呢 还是点状纹理
	if(m_ParentSystem->m_bUseTexture==true)
	{
		//同样  v=v0+at    s=so+vt
		m_fSpinSpeed += m_fSpinAcceleration*timePassed;
		m_fSpinAngle  += m_fSpinSpeed*timePassed;
		
		//弧度转过一圈了 就从来吧 一直加也没意思 数更大
		if(m_fSpinAngle>2*GLT_PI)
			m_fSpinAngle = m_fSpinAngle - 2*GLT_PI;
	}
	else 
		;
	m_fSize  += m_fSizeChange *timePassed;
}

void CFireParticle::Render()
{
	//点状纹理
	if (!m_ParentSystem->m_bUseTexture) 
	{
		
		gl->glPointSize(m_fSize);
		
		float color[4];
		color[0] = m_Color.x;
		color[1] = m_Color.y;
		color[2] = m_Color.z;
		color[3] = m_fAlpha;
		
		gl->glColorPointer(	4,   //(rgba)
						   GL_FLOAT,
						   0,
						   &color[0]);
		
		gl->glVertexPointer(	3,   //vertex (x,y,z)
							GL_FLOAT,
							0,   //跨距
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
		
		//纹理坐标
		GLshort coord[4][2]={0,0,1,0,1,1,0,1};
		
		float  lengthOF=(float)(m_fSize *1.414*0.707);
		float  xOF=(float)cos(m_fSpinAngle);
		float  yOF=(float)sin(m_fSpinAngle);
		
		//旋转后的以点位中心的 四边形的位置
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
		
		gl->glColorPointer(4,GL_FLOAT,0,&colorer[0][0]);
		
		gl->glTexCoordPointer(
							  2,
							  GL_SHORT,
							  0,
							  &coord[0][0]
							  );
		
		gl->glVertexPointer(	3,   //vertex (x,y,z)
							GL_FLOAT,
							0,   //跨距
							&vertex[0][0]);
		
		gl->glDrawArrays(GL_TRIANGLE_FAN,
						 0,
						 4);
		
	}
}

void CFireParticle::Deactive()
{
	m_fAge = 0.0f;
	m_bIsAlive = false;
	
	m_ParentSystem->m_iParticlesInUse--;
	return;
}

Vector3D CFireParticle::GetPosition()
{
	return -m_Position;
}

float CFireParticle::GetPower()
{
	float relativeAge = (m_fDieAge - m_fAge) / m_fDieAge;
	return 1-(relativeAge-1.0f/2.0f)*(relativeAge-1.0f/2.0f)*4.0;
	//return relativeAge;
}

void CFireParticle::OnDisturbWithMicrophone(double disturbing_coefficient_from_left, float timeStep)
{
	m_Velocity = m_Velocity + Vector3D(m_Acceleration.y, -m_Acceleration.x, m_Acceleration.z)*disturbing_coefficient_from_left*timeStep;
	//m_Position = m_Position + (m_Velocity*timeStep);
}