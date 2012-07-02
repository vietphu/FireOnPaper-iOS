//
//  PaperUnit.cpp
//  LogicDesign
//
//  Created by Han John on 12-4-12.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "PaperUnit.hpp"

PaperUnit::PaperUnit(int phlogistonQuantity)
{    
    //设置参数
    mTemperature = TPRT_ROOM;
    mPhlogistonQuantity = phlogistonQuantity;
    
    mIsBurning = false;
}

void PaperUnit::SetExist()
{
    mIsExist = true;
}

bool PaperUnit::GetIsExist()
{
    return mIsExist;
}

void PaperUnit::SetHandHold(bool isHandHold)
{
    mIsHandHold = isHandHold;
}

PaperUnit::~PaperUnit()
{
    
}
