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
    mHeatCapacity = HEAT_CAPACITY_SPACE;
    mHeatSpreadRatioSpaceToSpace = HEAT_SPREADRATIO_SPACE_TO_SPACE;
    mHeatSpreadRatioFireToSpace = HEAT_SPREADRATIO_FIRE_TO_SPACE;
    mHeatLossRatio = HEAT_LOSSRATIO;
    mMass = NORMALPAPER_MASS;
    
    mBurningRate = NORMALPAPER_PHLOGISTON_CONSUMERATE;
    mPhloHeatConvRatio = HEAT_PER_PHLOGISTON;
    mIgnitionTemperature = TPRT_NORMALPAPER_IGNITION;

    
    for (int y=0; y<PAPER_LOGIC_HEIGHT; y++)
        for (int x=0; x<PAPER_LOGIC_WIDTH; x++)
            mPaperUnits[y][x] = new PaperUnit(NORMALPAPER_PHLOGISTON_DENSITY);
    for (int y=0; y<SPACE_LOGIC_HEIGHT; y++)
        for (int x=0; x<SPACE_LOGIC_WIDTH; x++)
            mSpaceUnits[y][x] = new SpaceUnit();
    
    memset(mExchangeHeatBuffer, 0, sizeof(mExchangeHeatBuffer));
    
    mDrawTree = new DrawTree(0, 0, PAPER_LOGIC_WIDTH-1, PAPER_LOGIC_HEIGHT-1);
}

void Paper::Initialize(OpenGLES::OpenGLESContext *_gl, GLuint *_textureImageID, Env * env)
{
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
    for (int y=0; y<PAPER_LOGIC_HEIGHT; y++)
        for (int x=0; x<PAPER_LOGIC_WIDTH; x++)
            if (paperBitmap[y][x])
                mPaperUnits[y][x]->SetExist();

    mPaperUnits[handHoldPointY][handHoldPointX]->SetHandHold(true);
    
}

void Paper::SetPaperWorldSize(float width, float length, float lefttopX, float lefttopY)
{
    mPaperWorldWidth = width;
    mPaperWorldLength = length;
    mPaperWorldLeftTopX = lefttopX;
    mPaperWorldLeftTopY = lefttopY;
    
    GenVOB();
}

void Paper::DoPaperCreateHeat()
{
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
    for (int y=0; y<SPACE_LOGIC_HEIGHT; y++)
        for (int x=0; x<SPACE_LOGIC_WIDTH; x++)
        {
            mExchangeHeatBuffer[y][x] += mHeatSpreadRatioFireToSpace * mEnv->mFireParticlesDensity[y][x];
        }
}

void Paper::DoRefreshPaperStatus()
{
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
            }    
    
    memset(mExchangeHeatBuffer, 0, sizeof(mExchangeHeatBuffer));
}

void Paper::GivePaperFirstFire()
{
    mSpaceUnits[logicPaperY2LogicSpaceY(PAPER_LOGIC_HEIGHT-1)][logicPaperX2LogicSpaceX(PAPER_LOGIC_WIDTH-1)]->mTemperature += 900;
}

void Paper::TouchChangePaperMaterial(int spaceLogicX, int spaceLogicY)
{
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

bool Paper::IsValidPaperLogicCoord(int paperLogicX, int paperLogicY)
{
    if ( 0 <= paperLogicX && paperLogicX < PAPER_LOGIC_WIDTH && 0 <= paperLogicY && paperLogicY < PAPER_LOGIC_HEIGHT ) {
        return true;
    }
    return false;
}

bool Paper::IsValidSpaceLogicCoord(int spaceLogicX, int spaceLogicY)
{
    if ( 0 <= spaceLogicX && spaceLogicX < SPACE_LOGIC_WIDTH && 0 <= spaceLogicY && spaceLogicY < SPACE_LOGIC_HEIGHT ) {
        return true;
    }
    return false;
}

bool Paper::IsBuringBorder(int paperLogicX, int paperLogicY)
{
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
        vertex->Position = vec3((float)(x*widthperx + mPaperWorldLeftTopX), (float)(-y*lengthpery + mPaperWorldLeftTopY), -1.0);
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

