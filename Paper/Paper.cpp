//
//  Paper.cpp
//  FireOnPaper
//
//  Created by Han John on 12-5-17.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "Paper.hpp"

using namespace std;


Paper::Paper()
{
    //初始化所有的参数 包括物理模型的各个系数值和
    mHeatCapacity = HEAT_CAPACITY_SPACE;
    mHeatSpreadRatioSpaceToSpace = HEAT_SPREADRATIO_SPACE_TO_SPACE;
    mHeatSpreadRatioFireToSpace = HEAT_SPREADRATIO_FIRE_TO_SPACE;
    mHeatLossRatio = HEAT_LOSSRATIO;
    mMass = NORMALPAPER_MASS;
    
    mBurningRate = NORMALPAPER_PHLOGISTON_CONSUMERATE;
    mPhloHeatConvRatio = HEAT_PER_PHLOGISTON;
    mIgnitionTemperature = TPRT_NORMALPAPER_IGNITION;

    //初始化纸单元信息记录数组和空间单元信息记录数组
    for (int y=0; y<PAPER_LOGIC_HEIGHT; y++)
        for (int x=0; x<PAPER_LOGIC_WIDTH; x++)
            mPaperUnits[y][x] = new PaperUnit(NORMALPAPER_PHLOGISTON_DENSITY);
    for (int y=0; y<SPACE_LOGIC_HEIGHT; y++)
        for (int x=0; x<SPACE_LOGIC_WIDTH; x++)
            mSpaceUnits[y][x] = new SpaceUnit();
    
    //初始化热量交换缓存
    memset(mExchangeHeatBuffer, 0, sizeof(mExchangeHeatBuffer));
    
    //初始化二维线段树
    mDrawTree = new DrawTree(0, 0, PAPER_LOGIC_WIDTH-1, PAPER_LOGIC_HEIGHT-1);
}

void Paper::Initialize(OpenGLES::OpenGLESContext *_gl, GLuint *_textureImageID, Env * env)
{
    //设置gl上下文，纹理数组，与火焰交互的接口参数包
	gl = _gl;
	textureImageID = _textureImageID;
    mEnv = env;
}

void Paper::Render()
{    
    //Do Render
    gl->glEnableClientState(GL_VERTEX_ARRAY);
    gl->glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    gl->glEnable(GL_BLEND);
    gl->glDepthMask(GL_FALSE);
    gl->glEnable(GL_TEXTURE_2D);
    gl->glBindTexture(GL_TEXTURE_2D, textureImageID[0]);
    
     // Draw the paper
    int stride = sizeof(PaperVertex);
    const GLvoid* texCoordOffset = (const GLvoid*) (sizeof(vec3));
     
    //ARRAY BUFFER
    gl->glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    gl->glVertexPointer(3, GL_FLOAT, stride, 0);
    //TEXTURE BUFFER
    gl->glTexCoordPointer(2, GL_FLOAT, stride, texCoordOffset);
    
    set<DrawTreeNode*> nodeset = mDrawTree->getLeafNodeSet();
    vector<GLushort>::iterator index = mPaperIndices.begin();
    
    //根据drawtree中的信息，来决定要绘制哪些矩形
    //在这里会生成这些要绘制的矩形的索引数组
    int size = 0;
    int leftTopRectX;
    int leftTopRectY;
    int rightButtomRectX;
    int rightButtomRectY;
    const GLvoid* indices = &mPaperIndices[0];
    for (set<DrawTreeNode*>::iterator i = nodeset.begin(); i != nodeset.end(); i++)
    {
        leftTopRectX = (*i)->getX1();
        leftTopRectY = (*i)->getY1();
        rightButtomRectX = (*i)->getX2();
        rightButtomRectY = (*i)->getY2();
        *index++ = (PAPER_LOGIC_WIDTH+1)*leftTopRectY + leftTopRectX;
        *index++ = (PAPER_LOGIC_WIDTH+1)*leftTopRectY + rightButtomRectX + 1;
        *index++ = (PAPER_LOGIC_WIDTH+1)*(rightButtomRectY+1) + leftTopRectX;
        *index++ = (PAPER_LOGIC_WIDTH+1)*leftTopRectY + rightButtomRectX + 1;
        *index++ = (PAPER_LOGIC_WIDTH+1)*(rightButtomRectY+1) + leftTopRectX;
        *index++ = (PAPER_LOGIC_WIDTH+1)*(rightButtomRectY+1) + rightButtomRectX + 1;
        
        size++;
    }
    
    //绘制
    gl->glDrawElements(GL_TRIANGLES, 6*size, GL_UNSIGNED_SHORT, indices); 
    
    gl->glDisable(GL_BLEND);
    gl->glDepthMask(GL_TRUE);
    gl->glDisable(GL_TEXTURE_2D);
    gl->glDisableClientState(GL_VERTEX_ARRAY);
    gl->glDisableClientState(GL_TEXTURE_COORD_ARRAY);

}

void Paper::UpdateAnimation(float timeStep)
{
}

void Paper::OnRotateWithAccelerometer(double x, double y, double z)
{
}

void Paper::SetPaperLogicShape(bool paperBitmap[PAPER_LOGIC_HEIGHT][PAPER_LOGIC_WIDTH], int handHoldPointX, int handHoldPointY)
{    
    //对每一个需要设置为存在的纸片单元设置为存在
    for (int y=0; y<PAPER_LOGIC_HEIGHT; y++)
        for (int x=0; x<PAPER_LOGIC_WIDTH; x++)
            if (paperBitmap[y][x])
                mPaperUnits[y][x]->SetExist();

    mPaperUnits[handHoldPointY][handHoldPointX]->SetHandHold(true);
    
}

void Paper::SetPaperWorldSize(float width, float length, float lefttopX, float lefttopY)
{
    //设置纸片空间尺寸 并生成顶点数组
    mPaperWorldWidth = width;
    mPaperWorldLength = length;
    mPaperWorldLeftTopX = lefttopX;
    mPaperWorldLeftTopY = lefttopY;
    
    GenVOB();
}

void Paper::DoPaperCreateHeat()
{
    //遍历纸片数组 对每一个标记为正燃烧的，按照燃素消耗速度消耗燃素
    //此处还可以优化
    for (int y=0; y<PAPER_LOGIC_HEIGHT; y++)
        for (int x=0; x<PAPER_LOGIC_WIDTH; x++)
            if (mPaperUnits[y][x]->mIsBurning) 
            {
                mPaperUnits[y][x]->mPhlogistonQuantity -= mBurningRate;
                //HERE IS NOTHING TO DO
            }
}

void Paper::DoSpaceToSpaceHeatSpread()
{
    //对于每一个空间单元 进行热量的传导传递和辐射散失
    for (int y=0; y<SPACE_LOGIC_HEIGHT; y++)
        for (int x=0; x<SPACE_LOGIC_WIDTH; x++)
        {
            double dH;
            //传导过程
            //left
            if (IsValidSpaceLogicCoord(x-1, y))
            {
                dH = (double)(mSpaceUnits[y][x]->mTemperature - mSpaceUnits[y][x-1]->mTemperature) * mHeatSpreadRatioSpaceToSpace;
                if (dH < 0.0)
                    dH = 0.0;
                mExchangeHeatBuffer[y][x-1] += dH;
            }
            else
                dH = (double)(mSpaceUnits[y][x]->mTemperature - TPRT_ROOM) * mHeatSpreadRatioSpaceToSpace;
            mExchangeHeatBuffer[y][x] -= dH;
            //right
            if (IsValidSpaceLogicCoord(x+1, y)) 
            {
                dH = (double)(mSpaceUnits[y][x]->mTemperature - mSpaceUnits[y][x+1]->mTemperature) * mHeatSpreadRatioSpaceToSpace;
                if (dH < 0.0)
                    dH = 0.0;
                mExchangeHeatBuffer[y][x+1] += dH;
            }
            else
                dH = (double)(mSpaceUnits[y][x]->mTemperature - TPRT_ROOM) * mHeatSpreadRatioSpaceToSpace;
            mExchangeHeatBuffer[y][x] -= dH;
            //up
            if (IsValidSpaceLogicCoord(x, y-1))
            {
                dH = (double)(mSpaceUnits[y][x]->mTemperature - mSpaceUnits[y-1][x]->mTemperature) * mHeatSpreadRatioSpaceToSpace;
                if (dH < 0.0)
                    dH = 0.0;
                mExchangeHeatBuffer[y-1][x] += dH;
            }
            else
                dH = (double)(mSpaceUnits[y][x]->mTemperature - TPRT_ROOM) * mHeatSpreadRatioSpaceToSpace;
            mExchangeHeatBuffer[y][x] -= dH;
            //down
            if (IsValidSpaceLogicCoord(x, y+1))
            {
                dH = (double)(mSpaceUnits[y][x]->mTemperature - mSpaceUnits[y+1][x]->mTemperature) * mHeatSpreadRatioSpaceToSpace;
                if (dH < 0.0)
                    dH = 0.0;
                mExchangeHeatBuffer[y+1][x] += dH;
            }
            else
                dH = (double)(mSpaceUnits[y][x]->mTemperature - TPRT_ROOM) * mHeatSpreadRatioSpaceToSpace;
            mExchangeHeatBuffer[y][x] -= dH;
            //up left
            if (IsValidSpaceLogicCoord(x-1, y-1))
            {
                dH = (double)(mSpaceUnits[y][x]->mTemperature - mSpaceUnits[y-1][x-1]->mTemperature) * mHeatSpreadRatioSpaceToSpace;
                if (dH < 0.0)
                    dH = 0.0;
                mExchangeHeatBuffer[y-1][x-1] += dH/2;
            }
            else
                dH = (double)(mSpaceUnits[y][x]->mTemperature - TPRT_ROOM) * mHeatSpreadRatioSpaceToSpace;
            mExchangeHeatBuffer[y][x] -= dH/2;
            //up right
            if (IsValidSpaceLogicCoord(x+1, y-1))
            {
                dH = (double)(mSpaceUnits[y][x]->mTemperature - mSpaceUnits[y-1][x+1]->mTemperature) * mHeatSpreadRatioSpaceToSpace;
                if (dH < 0.0)
                    dH = 0.0;
                mExchangeHeatBuffer[y-1][x+1] += dH/2;
            }
            else
                dH = (double)(mSpaceUnits[y][x]->mTemperature - TPRT_ROOM) * mHeatSpreadRatioSpaceToSpace;
            mExchangeHeatBuffer[y][x] -= dH/2;
            //down left
            if (IsValidSpaceLogicCoord(x-1, y+1))
            {
                dH = (double)(mSpaceUnits[y][x]->mTemperature - mSpaceUnits[y+1][x-1]->mTemperature) * mHeatSpreadRatioSpaceToSpace;
                if (dH < 0.0)
                    dH = 0.0;
                mExchangeHeatBuffer[y+1][x-1] += dH/2;
            }
            else
                dH = (double)(mSpaceUnits[y][x]->mTemperature - TPRT_ROOM) * mHeatSpreadRatioSpaceToSpace;
            mExchangeHeatBuffer[y][x] -= dH/2;
            //down right
            if (IsValidSpaceLogicCoord(x+1, y+1))
            {
                dH = (double)(mSpaceUnits[y][x]->mTemperature - mSpaceUnits[y+1][x+1]->mTemperature) * mHeatSpreadRatioSpaceToSpace;
                if (dH < 0.0)
                    dH = 0.0;
                mExchangeHeatBuffer[y+1][x+1] += dH/2;
            }
            else
                dH = (double)(mSpaceUnits[y][x]->mTemperature - TPRT_ROOM) * mHeatSpreadRatioSpaceToSpace;
            mExchangeHeatBuffer[y][x] -= dH/2;
                        
            //热损失过程
            //辐射热损失
            double T = (double)mSpaceUnits[y][x]->mTemperature;
            mExchangeHeatBuffer[y][x] -= (T*T*T*T)*mHeatLossRatio;
        }
}

void Paper::DoFireToSpaceHeatSpread()
{
    //从和火焰交互传递对象env中，得知火焰位置，并且对火焰位置对应的空间单元，按照预置系数进行升温
    for (int y=0; y<SPACE_LOGIC_HEIGHT; y++)
        for (int x=0; x<SPACE_LOGIC_WIDTH; x++)
        {
            mExchangeHeatBuffer[y][x] += mHeatSpreadRatioFireToSpace * mEnv->mFireParticlesDensity[y][x];
        }
}

void Paper::DoRefreshPaperStatus()
{
    //更新纸片状态
    //包括从热量缓存中将热量交换到空间单元中
    //和将空间单元中将温度交换到对应格点的纸片单元中
    //并且此时要测试纸片是否燃尽和测试纸片是否被点燃
    for (int y=0; y<SPACE_LOGIC_HEIGHT; y++)
    {
        for (int x=0; x<SPACE_LOGIC_WIDTH; x++)
        {
            mSpaceUnits[y][x]->mTemperature += mExchangeHeatBuffer[y][x]/mHeatCapacity;
            //cout<<mSpaceUnits[y][x]->mTemperature<<" ";
        }
        //cout<<endl;
    }
    
    for (int y=0; y<PAPER_LOGIC_HEIGHT; y++)
        for (int x=0; x<PAPER_LOGIC_WIDTH; x++)
            if (mPaperUnits[y][x]->mIsExist)
            {
                mPaperUnits[y][x]->mTemperature = mSpaceUnits[logicPaperY2LogicSpaceY(y)][logicPaperX2LogicSpaceX(x)]->mTemperature;
                if (mPaperUnits[y][x]->mIsBurning && mPaperUnits[y][x]->mPhlogistonQuantity <= 0) 
                {
                    if (IsBuringBorder(x, y)) 
                    {
                        mEnv->ResetSpaceFire(logicPaperX2LogicSpaceX(x), logicPaperY2LogicSpaceY(y));
                        if (IsValidPaperLogicCoord(x-1, y-1) && mPaperUnits[y-1][x-1]->mIsBurning) {
                            mEnv->SetSpaceFire(logicPaperX2LogicSpaceX(x-1), logicPaperY2LogicSpaceY(y-1));
                        }
                        if (IsValidPaperLogicCoord(x, y-1) && mPaperUnits[y-1][x]->mIsBurning) {
                            mEnv->SetSpaceFire(logicPaperX2LogicSpaceX(x), logicPaperY2LogicSpaceY(y-1));
                        }
                        if (IsValidPaperLogicCoord(x+1, y-1) && mPaperUnits[y-1][x+1]->mIsBurning) {
                            mEnv->SetSpaceFire(logicPaperX2LogicSpaceX(x+1), logicPaperY2LogicSpaceY(y-1));
                        }
                        if (IsValidPaperLogicCoord(x-1, y) && mPaperUnits[y][x-1]->mIsBurning) {
                            mEnv->SetSpaceFire(logicPaperX2LogicSpaceX(x-1), logicPaperY2LogicSpaceY(y));
                        }
                        if (IsValidPaperLogicCoord(x+1, y) && mPaperUnits[y][x+1]->mIsBurning) {
                            mEnv->SetSpaceFire(logicPaperX2LogicSpaceX(x+1), logicPaperY2LogicSpaceY(y));
                        }
                        if (IsValidPaperLogicCoord(x-1, y+1) && mPaperUnits[y+1][x-1]->mIsBurning) {
                            mEnv->SetSpaceFire(logicPaperX2LogicSpaceX(x-1), logicPaperY2LogicSpaceY(y+1));
                        }
                        if (IsValidPaperLogicCoord(x, y+1) && mPaperUnits[y+1][x]->mIsBurning) {
                            mEnv->SetSpaceFire(logicPaperX2LogicSpaceX(x), logicPaperY2LogicSpaceY(y+1));
                        }
                        if (IsValidPaperLogicCoord(x+1, y+1) && mPaperUnits[y+1][x+1]->mIsBurning) {
                            mEnv->SetSpaceFire(logicPaperX2LogicSpaceX(x+1), logicPaperY2LogicSpaceY(y+1));
                        }
                    };
                    mPaperUnits[y][x]->mIsBurning = false;
                    mPaperUnits[y][x]->mIsExist = false;
                    mDrawTree->change(x, y);
                    continue;
                }
                if (!mPaperUnits[y][x]->mIsBurning && mPaperUnits[y][x]->mTemperature >= mIgnitionTemperature) 
                {
                    mPaperUnits[y][x]->mIsBurning = true;
                    if (IsBuringBorder(x, y)) 
                    {
                        mEnv->SetSpaceFire(logicPaperX2LogicSpaceX(x), logicPaperY2LogicSpaceY(y));
                    }
                }
                if (!mPaperUnits[y][x]->mIsBurning) {
                    //通过切向差值方法检测边缘，以调整圆滑度，减少锯齿感
                    if (IsNotDrawThisPoint(x, y)) {
                        mPaperUnits[y][x]->mPhlogistonQuantity = 0;
                        mPaperUnits[y][x]->mIsBurning = false;
                        mPaperUnits[y][x]->mIsExist = false;
                        mDrawTree->change(x, y);
                    }
                }
            }    
    
    memset(mExchangeHeatBuffer, 0, sizeof(mExchangeHeatBuffer));
}

void Paper::GivePaperFirstFire()
{
    //给予纸片左下角一个初始的热量
    mSpaceUnits[logicPaperY2LogicSpaceY(PAPER_LOGIC_HEIGHT-1)][logicPaperX2LogicSpaceX(PAPER_LOGIC_WIDTH-1)]->mTemperature += 900;
    mSpaceUnits[logicPaperY2LogicSpaceY(PAPER_LOGIC_HEIGHT-2)][logicPaperX2LogicSpaceX(PAPER_LOGIC_WIDTH-1)]->mTemperature += 900;
    mSpaceUnits[logicPaperY2LogicSpaceY(PAPER_LOGIC_HEIGHT-1)][logicPaperX2LogicSpaceX(PAPER_LOGIC_WIDTH-2)]->mTemperature += 900;
    mSpaceUnits[logicPaperY2LogicSpaceY(PAPER_LOGIC_HEIGHT-3)][logicPaperX2LogicSpaceX(PAPER_LOGIC_WIDTH-1)]->mTemperature += 900;
    mSpaceUnits[logicPaperY2LogicSpaceY(PAPER_LOGIC_HEIGHT-1)][logicPaperX2LogicSpaceX(PAPER_LOGIC_WIDTH-3)]->mTemperature += 900;
}

void Paper::TouchChangePaperMaterial(int spaceLogicX, int spaceLogicY)
{
    //对于触摸位置，若当格纸片未被烧尽，则重新设置其燃素含量
    //cout<<"Space logic position at: "<<spaceLogicX<<" "<<spaceLogicY<<endl;
    int paper_logic_x = logicSpaceX2LogicPaperX(spaceLogicX);
    int paper_logic_y = logicSpaceY2LogicPaperY(spaceLogicY);
    if (IsValidPaperLogicCoord(paper_logic_x, paper_logic_y)) 
    {
        if (mPaperUnits[paper_logic_y][paper_logic_x]->mIsExist) 
        {
            mPaperUnits[paper_logic_y][paper_logic_x]->mPhlogistonQuantity = TOUCH_SET_PHLOGISTON_DENSITY;
            //cout<<"add fire at: "<<paper_logic_x<<" "<<paper_logic_y<<endl;
        }
    }
}

void Paper::TouchPaperToBurn(int spaceLogicX, int spaceLogicY)
{
    //对于触摸位置，将改点和改点周围的空间单元的温度进行提升
    //因而有可能将导致周围若存在的纸片发生燃烧
    int paper_logic_x = logicSpaceX2LogicPaperX(spaceLogicX);
    int paper_logic_y = logicSpaceY2LogicPaperY(spaceLogicY);
    if (IsValidPaperLogicCoord(paper_logic_x, paper_logic_y)) 
    {
        if (mPaperUnits[paper_logic_y][paper_logic_x]->mIsExist) 
        {
            mPaperUnits[paper_logic_y][paper_logic_x]->mPhlogistonQuantity = 0;
            mPaperUnits[paper_logic_y][paper_logic_x]->mIsExist = false;
            mPaperUnits[paper_logic_y][paper_logic_x]->mIsBurning = false;
            mEnv->ResetSpaceFire(spaceLogicX, spaceLogicY);
            mDrawTree->change(paper_logic_x, paper_logic_y);
            
            int x,y;
            
            x = spaceLogicX-1;
            y = spaceLogicY-1;
            if (IsValidSpaceLogicCoord(x, y))
                mSpaceUnits[y][x]->mTemperature += 900;
            
            x = spaceLogicX-1;
            y = spaceLogicY;
            if (IsValidSpaceLogicCoord(x, y))
                mSpaceUnits[y][x]->mTemperature += 900;
            
            x = spaceLogicX-1;
            y = spaceLogicY+1;
            if (IsValidSpaceLogicCoord(x, y))
                mSpaceUnits[y][x]->mTemperature += 900;
            
            x = spaceLogicX;
            y = spaceLogicY-1;
            if (IsValidSpaceLogicCoord(x, y))
                mSpaceUnits[y][x]->mTemperature += 900;
            
            x = spaceLogicX;
            y = spaceLogicY+1;
            if (IsValidSpaceLogicCoord(x, y))
                mSpaceUnits[y][x]->mTemperature += 900;
            
            x = spaceLogicX+1;
            y = spaceLogicY-1;
            if (IsValidSpaceLogicCoord(x, y))
                mSpaceUnits[y][x]->mTemperature += 900;
            
            x = spaceLogicX+1;
            y = spaceLogicY;
            if (IsValidSpaceLogicCoord(x, y))
                mSpaceUnits[y][x]->mTemperature += 900;
            
            x = spaceLogicX+1;
            y = spaceLogicY+1;
            if (IsValidSpaceLogicCoord(x, y))
                mSpaceUnits[y][x]->mTemperature += 900;
            
        }
    }
}

bool Paper::IsValidPaperLogicCoord(int paperLogicX, int paperLogicY)
{
    //若超过上下限即为不合法的坐标
    if ( 0 <= paperLogicX && paperLogicX < PAPER_LOGIC_WIDTH && 0 <= paperLogicY && paperLogicY < PAPER_LOGIC_HEIGHT ) {
        return true;
    }
    return false;
}

bool Paper::IsValidSpaceLogicCoord(int spaceLogicX, int spaceLogicY)
{
    //若超过上下限即为不合法的坐标
    if ( 0 <= spaceLogicX && spaceLogicX < SPACE_LOGIC_WIDTH && 0 <= spaceLogicY && spaceLogicY < SPACE_LOGIC_HEIGHT ) {
        return true;
    }
    return false;
}

bool Paper::IsBuringBorder(int paperLogicX, int paperLogicY)
{
    //若该点正燃烧，并且周围有点的纸单元不存在，则这就是一个燃烧边缘
    if (!mPaperUnits[paperLogicY][paperLogicX]->mIsBurning) {
        return false;
    }
    if ( !IsValidPaperLogicCoord( paperLogicX+1, paperLogicY) || !IsValidPaperLogicCoord( paperLogicX-1, paperLogicY) || !IsValidPaperLogicCoord( paperLogicX, paperLogicY+1) || !IsValidPaperLogicCoord( paperLogicX, paperLogicY-1) ) {
        return true;
    }
    if (!mPaperUnits[paperLogicY-1][paperLogicX]->mIsExist || !mPaperUnits[paperLogicY+1][paperLogicX]->mIsExist || !mPaperUnits[paperLogicY][paperLogicX-1]->mIsExist || !mPaperUnits[paperLogicY][paperLogicX+1]->mIsExist || !mPaperUnits[paperLogicY-1][paperLogicX-1]->mIsExist || !mPaperUnits[paperLogicY+1][paperLogicX+1]->mIsExist || !mPaperUnits[paperLogicY+1][paperLogicX-1]->mIsExist || !mPaperUnits[paperLogicY-1][paperLogicX+1]->mIsExist) {
        return true;
    }
    return false;
}

//测试当前的切小纸片单元是否应该被绘制 默认传入的坐标是合法的
bool Paper::IsNotDrawThisPoint(int paperLogicX, int paperLogicY)
{
    if (!IsValidPaperLogicCoord(paperLogicX-1, paperLogicY-1) ||
        !IsValidPaperLogicCoord(paperLogicX-1, paperLogicY+1) ||
        !IsValidPaperLogicCoord(paperLogicX+1, paperLogicY-1) ||
        !IsValidPaperLogicCoord(paperLogicX+1, paperLogicY+1) ) 
        return false;
    
    if ( !mPaperUnits[paperLogicY-1][paperLogicX-1]->mIsExist &&
        !mPaperUnits[paperLogicY+1][paperLogicX+1]->mIsExist )
        return true;

    if ( !mPaperUnits[paperLogicY-1][paperLogicX+1]->mIsExist &&
        !mPaperUnits[paperLogicY+1][paperLogicX-1]->mIsExist )
        return true;
    
    return false;
}

bool Paper::GetPaperIsFire(int paperLogicX, int paperLogicY)
{
    return mPaperUnits[paperLogicY][paperLogicX]->mIsBurning;
}

void Paper::GenVOB()
{    
    mPaperIndexCount = PAPER_LOGIC_HEIGHT * PAPER_LOGIC_WIDTH * 6;
    int vertexCount = ( PAPER_LOGIC_HEIGHT + 1 ) * ( PAPER_LOGIC_WIDTH + 1 );
    mPaperVertices.resize(vertexCount);
    vector<PaperVertex>::iterator vertex = mPaperVertices.begin();

    //Gen paper vertex position
    float widthperx = mPaperWorldWidth / PAPER_LOGIC_WIDTH;
    float lengthpery = mPaperWorldLength / PAPER_LOGIC_HEIGHT;
    int x = 0;
    int y = 0;
    while ( vertex != mPaperVertices.end() ) 
    {
        vertex->Position = vec3((float)(x*widthperx + mPaperWorldLeftTopX), (float)(-y*lengthpery + mPaperWorldLeftTopY), 0.0);
        vertex->Texture = vec2((float)x/(float)PAPER_LOGIC_WIDTH, (float)y/(float)PAPER_LOGIC_HEIGHT);
                
        vertex++;
        x++;
        if ( x == PAPER_LOGIC_WIDTH+1 ) 
        {
            y++;
            x = 0;
        }
    }

    // Create the VBO for the vertices.
    gl->glGenBuffers(1, &mVertexBuffer);
    gl->glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    gl->glBufferData(GL_ARRAY_BUFFER,
                 mPaperVertices.size() * sizeof(mPaperVertices[0]),
                 &mPaperVertices[0],
                 GL_STATIC_DRAW);

    mPaperIndices.resize(PAPER_LOGIC_WIDTH * PAPER_LOGIC_HEIGHT * 6);
}

Paper::~Paper()
{
    //删除所有的纸片单元和空间单元
    for (int y=0; y<PAPER_LOGIC_HEIGHT; y++)
        for (int x=0; x<PAPER_LOGIC_WIDTH; x++)
            if (mPaperUnits[y][x])
            {
                delete mPaperUnits[y][x];
                mPaperUnits[y][x] = NULL;
            }
    for (int y=0; y<SPACE_LOGIC_HEIGHT; y++)
        for (int x=0; x<SPACE_LOGIC_WIDTH; x++)
            if (mSpaceUnits[y][x])
            {
                delete mSpaceUnits[y][x];
                mSpaceUnits[y][x] = NULL;
            }
    delete mDrawTree;
}

