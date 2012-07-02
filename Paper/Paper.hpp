//
//  Paper.hpp
//  FireOnPaper
//
//  Created by Han John on 12-5-17.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef FireOnPaper_Paper_hpp
#define FireOnPaper_Paper_hpp

#include "Parameters.hpp"
#include "OpenGLESContext.hpp"
#include <iostream>
#include "Vector.hpp"
#include "Matrix.hpp"
#include <vector>
#include "FireOnPaperEnv.hpp"
#include "SpaceUnit.hpp"
#include "PaperUnit.hpp"
#include "DrawTree.hpp"

using namespace std;
struct PaperVertex {
    vec3 Position;
    vec2 Texture;
};

#define PAPER_TEXTURE_ID 1

//纸片类，记录所有与纸片有关的属性，及进行的相关操作
class Paper
{
public:
    
    
    //LC MOVE IN  在纸片逻辑坐标x，y处给予纸片一个初始的热量让其燃烧 已废弃
    bool giveFirstHeat(int x, int y, int heat);
    
    //LC MOVE IN END
    
    //CALL FROM OUT
    
    Paper();
    //初始化 需要传入一个上下文 一个纹理数组 一个和火焰交互使用的参数包，我们称为环境
    void Initialize(OpenGLES::OpenGLESContext * _gl, GLuint * _textureImageID, Env * env);
    //绘制函数 
    void Render();
    //更新下一帧内容函数
    void UpdateAnimation(float timeStep);
    //重力传感器调用函数
	void OnRotateWithAccelerometer(double x, double y, double z);
    
    //设置初始纸张的逻辑形状
    //传入一个位图，来标记初始的纸片每个位置的存在情况，以决定其形状，并且需要设置一个手持点（目前还没用上）
    void SetPaperLogicShape(bool paperBitmap[PAPER_LOGIC_HEIGHT][PAPER_LOGIC_WIDTH], int handHoldPointX, int handHoldPointY);
    //设置纸片在世界坐标中的位置和大小
    //传入参数为 宽 高 左上角坐标x，y值
    void SetPaperWorldSize(float width, float length, float lefttopX, float lefttopY);
    //纸燃烧产生热量过程
    void DoPaperCreateHeat();
    //热量在空间中传递过程
    void DoSpaceToSpaceHeatSpread();
    //热量由火传递到空间过程
    void DoFireToSpaceHeatSpread();
    //将当格空间单元的热量传递到纸片，以更新纸片温度
    void DoRefreshPaperStatus();   //更新温度
    
    //给予纸片一个初始火焰的过程
    void GivePaperFirstFire();
    
    //当触摸改变纸片材质，使其燃烧更持久时，调用此函数
    //传入参数为 空间逻辑坐标x，y
    void TouchChangePaperMaterial(int spaceLogicX, int spaceLogicY);
    
    //当触摸使纸片开始燃烧，调用此函数
    //传入参数为 空间逻辑坐标x，y
    void TouchPaperToBurn(int spaceLogicX, int spaceLogicY);
    
    ~Paper();
    
    
    
    
private:
	OpenGLES::OpenGLESContext * gl; //opengl上下文
    Env * mEnv;                     //和火焰交互使用的参数包
	GLuint * textureImageID;        //纹理数组，因为纹理图的读入和设备有关，因此需要外部生成之后传入
    
    DrawTree * mDrawTree;           //优化纸片绘制使用的二维线段树结构

    PaperUnit * mPaperUnits[PAPER_LOGIC_HEIGHT][PAPER_LOGIC_WIDTH]; //所有空间单元管理数组
    SpaceUnit * mSpaceUnits[SPACE_LOGIC_HEIGHT][SPACE_LOGIC_WIDTH]; //所有纸片单元管理数组
    double mExchangeHeatBuffer[SPACE_LOGIC_HEIGHT][SPACE_LOGIC_WIDTH];  //用于进行热量传递的缓存
                                                                        //热量传递需要分两步进行，第一步需要存入缓存
    //纸片在世界坐标中的实际尺寸和位置
    float mPaperWorldWidth;
    float mPaperWorldLength;
    float mPaperWorldLeftTopX;
    float mPaperWorldLeftTopY;

    //纸单元参数
    int mMass;                  //纸单元的质量
    
    int mBurningRate;           //一个时间单位内，燃烧消耗多少燃素
    int mPhloHeatConvRatio;     //每一个燃素的消耗，能够转化成多少热量
    int mIgnitionTemperature;   //纸单元着火点
    
    int mHeatCapacity;          //空间的比热容
    double mHeatSpreadRatioFireToSpace;
    double mHeatSpreadRatioSpaceToSpace; //空间热传播系数，传导辐射，简化处理
    //此时使用的热传播方程是 dH = c（T1 - T2）
    double mHeatLossRatio;     //空间的热散失系数，主要是通过辐射方式，散失到我们讨论范围之外空间的热量，同样简化处理
    //此处使用的热散失方程式 dH = c (T)^4
        
    //图形绘制相关的定点缓冲和索引缓冲 索引缓冲在优化drawtree绘制之后，废弃不用
    GLuint mVertexBuffer;
    GLuint mIndexBuffer;
    
    //图形绘制相关的顶点数组和索引数组
    std::vector<PaperVertex> mPaperVertices;
    std::vector<GLushort> mPaperIndices;
    int mPaperIndexCount;   //索引数组尺寸
        
    //工具函数 测试是否是一个合法的纸单元逻辑坐标
    bool IsValidPaperLogicCoord(int paperLogicX, int paperLogicY);
    //工具函数 测试是否是一个合法的空间单元逻辑坐标
    bool IsValidSpaceLogicCoord(int spaceLogicX, int spaceLogicY);
    //工具函数 测试是否是一个燃烧边缘 传入参数为纸片逻辑坐标
    bool IsBuringBorder(int paperLogicX, int paperLogicY);
    //工具函数 测试当前纸片单元是否已经着火 传入参数为纸片逻辑坐标
    bool GetPaperIsFire(int paperLogicX, int paperLogicY);
    //生成顶点数组，以备顶点索引进行调用
    void GenVOB();
   
};


#endif
