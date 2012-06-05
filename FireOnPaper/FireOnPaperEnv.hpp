//
//  FireOnPaperEnv.h
//  FireOnPaper
//
//  Created by Wang Zhongyu on 12-6-5.
//  Copyright (c) 2012年 Peking University. All rights reserved.
//

#ifndef FireOnPaper_FireOnPaperEnv_h
#define FireOnPaper_FireOnPaperEnv_h

#include "Parameters.hpp"
#ifdef __cplusplus
#include <bitset>
#endif

using namespace std;

#define logicSpaceX2LogicPaperX(x) (x-PAPER_SPACE_ADJUST_WIDTH)
#define logicSpaceY2LogicPaperY(y) (y-PAPER_SPACE_ADJUST_HEIGHT)
#define logicPaperX2LogicSpaceX(x) (x+PAPER_SPACE_ADJUST_WIDTH)
#define logicPaperY2LogicSpaceY(y) (y+PAPER_SPACE_ADJUST_HEIGHT)
#define logicSpaceX2ScreenX(x) (float)(-(float)SCREEN_WIDTH/2+(float)x*SCREEN_WIDTH/(float)SPACE_LOGIC_WIDTH)
#define logicSpaceY2ScreenY(y) (float)((float)SCREEN_HEIGHT/2-(float)y*SCREEN_HEIGHT/(float)SPACE_LOGIC_HEIGHT)
#define logicPaperX2ScreenX(x) (float)(-(float)SCREEN_WIDTH/2+(float)logicPaperX2LogicSpaceX(x)*SCREEN_WIDTH/(float)SPACE_LOGIC_WIDTH)
#define logicPaperY2ScreenY(y) (float)((float)SCREEN_HEIGHT/2-(float)logicPaperY2LogicSpaceY(y)*SCREEN_HEIGHT/(float)SPACE_LOGIC_HEIGHT)

#define screenX2LogicSpaceX(x) (int)((x+(SCREEN_WIDTH)/2.0)/SCREEN_WIDTH_PER_UNIT)
#define screenY2LogicSpaceY(y) (int)(((SCREEN_HEIGHT)/2.0-y)/SCREEN_HEIGHT_PER_UNIT)


class Env
{
public:
    bitset<SPACE_LOGIC_HEIGHT*SPACE_LOGIC_WIDTH> mSpaceIsFireBitMap;
    
    float mFireParticlesDensity[SPACE_LOGIC_HEIGHT][SPACE_LOGIC_WIDTH];
	
    Env() 
    {
		memset(mFireParticlesDensity, 0, sizeof(mFireParticlesDensity));
    }
    void SetSpaceFire(int _spaceLogicX, int _spaceLogicY)
    {
        mSpaceIsFireBitMap.set(_spaceLogicY*SPACE_LOGIC_WIDTH+_spaceLogicX);
    }
    void ResetSpaceFire(int _spaceLogicX, int _spaceLogicY)
    {
        mSpaceIsFireBitMap.reset(_spaceLogicY*SPACE_LOGIC_WIDTH+_spaceLogicX);
    }
    bool GetSpaceFire(int _spaceLogicX, int _spaceLogicY)
    {
		int tmp = _spaceLogicY*SPACE_LOGIC_WIDTH+_spaceLogicX;
        return mSpaceIsFireBitMap[tmp];
    }
};

#endif
