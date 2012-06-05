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

class Paper
{
public:
    
    
    //LC MOVE IN    
    bool giveFirstHeat(int x, int y, int heat);
    
    //LC MOVE IN END
    
    //CALL FROM OUT
    
    Paper();
    void Initialize(OpenGLES::OpenGLESContext * _gl, GLuint * _textureImageID, Env * env);
    void Render();
    void UpdateAnimation(float timeStep);
	void OnRotateWithAccelerometer(double x, double y, double z);
    
    void SetPaperLogicShape(bool paperBitmap[PAPER_LOGIC_HEIGHT][PAPER_LOGIC_WIDTH], int handHoldPointX, int handHoldPointY);
    void SetPaperWorldSize(float width, float length, float lefttopX, float lefttopY);
    void DoPaperCreateHeat();
    void DoSpaceToSpaceHeatSpread();
    void DoFireToSpaceHeatSpread();
    void DoRefreshPaperStatus();   //更新温度
    
    void GivePaperFirstFire();
    
    void TouchChangePaperMaterial(int spaceLogicX, int spaceLogicY);
    
    ~Paper();
    
    
    
    
private:
	OpenGLES::OpenGLESContext * gl;
    Env * mEnv;
	GLuint * textureImageID;
    
    DrawTree * mDrawTree;

    PaperUnit * mPaperUnits[PAPER_LOGIC_HEIGHT][PAPER_LOGIC_WIDTH];
    SpaceUnit * mSpaceUnits[SPACE_LOGIC_HEIGHT][SPACE_LOGIC_WIDTH];
    double mExchangeHeatBuffer[SPACE_LOGIC_HEIGHT][SPACE_LOGIC_WIDTH];
    
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
        
    GLuint mVertexBuffer;
    GLuint mIndexBuffer;
    
    std::vector<PaperVertex> mPaperVertices;
    std::vector<GLushort> mPaperIndices;
    int mPaperIndexCount;
        
    bool IsValidPaperLogicCoord(int paperLogicX, int paperLogicY);
    bool IsValidSpaceLogicCoord(int spaceLogicX, int spaceLogicY);
    bool IsBuringBorder(int paperLogicX, int paperLogicY);
    bool GetPaperIsFire(int paperLogicX, int paperLogicY);
    void GenVOB();
   
};


#endif
