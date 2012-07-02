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

//记录一个纸片单元的各个属性
class PaperUnit
{
public:

    int mTemperature;           //纸单元的温度
    int mPhlogistonQuantity;    //纸单元中的燃素含量
    
    //记录纸片单元的三个状态 是否燃烧 是否存在 是否被手持（未使用）
    bool mIsBurning;
    bool mIsExist;
    bool mIsHandHold;

    //初始化，需要设定纸片单元的一些参数
    PaperUnit(int phlogistonQuantity);
    //设置纸片单元为存在
    void SetExist();
    //获取纸片单元是否存在
    bool GetIsExist();
    //设置纸片单元被手持
    void SetHandHold(bool isHandHold);
    ~PaperUnit();
};


#endif
