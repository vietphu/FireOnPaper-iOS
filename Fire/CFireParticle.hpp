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

#define RANDOM_FLOAT (((float)rand())/RAND_MAX)
#define GLT_PI 3.14159265358979323846

class CFireParticleSystem;

class CFireParticle
{
private:
	
    OpenGLES::OpenGLESContext *gl;
	
	//位置
	Vector3D m_Position;
	//速度
	Vector3D m_Velocity;
	//加速度  
	Vector3D m_Acceleration;
	
	//透明度 和 变化率
	float m_fAlpha;
	float m_fAlphaChange; 
	
	//同理  颜色 和 颜色变化率
	Vector3D m_Color;   
	Vector3D m_ColorChange; 
	
	//生命周期  当前年龄
	float m_fDieAge;
	float m_fAge;	
	
	/*--------------------------*
	 *  m_bUseTexture=TRUE		*
	 *  时候  使用多边形  旋转		*
	 *--------------------------*/
	//多边形纹理   旋转的角度  角速度  角加速度
	float m_fSpinAngle; 
	float m_fSpinSpeed;
	float m_fSpinAcceleration;
	
	/*--------------------------*
	 *  m_bUseTexture=FALSE		*
	 *  时候  使用点状纹理			*
	 *--------------------------*/
	//点状纹理   点的大小   大小变化率
	float m_fSize;
	float m_fSizeChange;	
	
	//从整体系统获取初始化数据
	CFireParticleSystem * m_ParentSystem;
	
public:
	CFireParticle();
	bool m_bIsAlive;
	void Initialize(CFireParticleSystem * ParentSystem, OpenGLES::OpenGLESContext *_gl, int x, int y);
	void Update(float timePassed, int x, int y);
	void Render();
	void Deactive();
	Vector3D GetPosition();
	float GetPower();
	void OnDisturbWithMicrophone(double disturbing_coefficient_from_left, float timeStep);
};

class CFireParticleSystem
{
private:
	OpenGLES::OpenGLESContext * gl;
	GLuint * textureImageID;
	Env * env;
	GLuint m_vertexBuffer;
	int point_number;
	CFireParticle * fire_particles;
	int * particle_x, * particle_y;
	int last_particle_initialized;
	bool has_direction, from_left;
	double disturbing_coefficient;
	double totalTime;
	int signal;
	double change_cycle;
	
public:
	/*--------------------------*
	 *  火焰粒子发射参数			*
	 *--------------------------*/
	//发射的地方
	Vector3D m_EmitterPosition;
	//发射半径内的偏移
	Vector3D m_CreationDeviation;  
	//发射的方向
	Vector3D m_EmitDirection;
	//发射方向的偏移 （当然不可能都是一个方向对吧）
	Vector3D m_EmitDirectionDeviation; 
	//发射的最小最大速度
	float m_fMinEmitSpeed;
	float m_fMaxEmitSpeed;	
	//加速度方向
	Vector3D m_AccelerationDirection;
	//最小最大加速度
	float m_fMinAcceleration;
	float m_fMaxAcceleration;
	//最小最大  发射  死亡  时候的 透明度
	float m_fMinEmitAlpha;
	float m_fMaxEmitAlpha;
	float m_fMinDieAlpha;
	float m_fMaxDieAlpha;
	//最小最大   发射 死亡时候的颜色（RGB）
	Vector3D m_MinEmitColor;
	Vector3D m_MaxEmitColor;
	Vector3D m_MinDieColor;
	Vector3D m_MaxDieColor;	
	//最小最大   死亡时间
	float m_fMinDieAge;
	float m_fMaxDieAge;
	
	/*--------------------------*
	 *  m_bUseTexture=TRUE		*
	 *  时候  使用多边形  旋转		*
	 *--------------------------*/
	//发射的旋转速度
	float m_fMinEmitSpinSpeed;
	float m_fMaxEmitSpinSpeed;
	//旋转角加速度
	float m_fMinSpinAcceleration;
	float m_fMaxSpinAcceleration;
	
	/*--------------------------*
	 *  m_bUseTexture=FALSE		*
	 *  时候  使用点状纹理			*
	 *--------------------------*/
	//最小最大 发射 死亡时候的大小
	float m_fMinEmitSize;
	float m_fMaxEmitSize;
	float m_fMinDieSize;
	float m_fMaxDieSize;
	
	/*--------------------------*
	 *  关于系统的其他参数			*
	 *--------------------------*/	
	//当粒子死亡了   还新建么
	bool m_bRecreateWhenDied;
	//是用纹理图  还是用点状纹理
	bool m_bUseTexture;
	//创建的时候容纳多少个粒子
	int m_iMaxParticles;
	//当前使用中的有多少个
	int m_iParticlesInUse;
	
	CFireParticleSystem();
	
    void Initialize(OpenGLES::OpenGLESContext * _gl, GLuint * _textureImageID, Env * _env, int _point_number);
	void BeforeRender();
    void Render();
	void AfterRender();
    void UpdateAnimation(float timeStep);
	void OnRotateWithAccelerometer(double x, double y, double z);
	void OnDisturbWithMicrophone(bool _has_direction, bool _from_left, double _disturbing_coefficient);
	
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
