//
//  PaperUnit.h
//  LogicDesign
//
//  Created by Han John on 12-4-12.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef LogicDesign_PaperUnit_h
#define LogicDesign_PaperUnit_h

#include "Parameters.hpp"

class PaperUnit
{
public:

    int mTemperature;           //纸单元的温度
    int mPhlogistonQuantity;    //纸单元中的燃素含量
    
    bool mIsBurning;
    bool mIsExist;
    bool mIsHandHold;

    //初始化，需要设定纸片单元的一些参数
    PaperUnit(int phlogistonQuantity);
    void SetExist();
    bool GetIsExist();
    void SetHandHold(bool isHandHold);
    ~PaperUnit();
};


#endif
