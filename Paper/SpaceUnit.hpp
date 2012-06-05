//
//  SpaceUnit.h
//  LogicDesign
//
//  Created by Han John on 12-4-12.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef LogicDesign_SpaceUnit_h
#define LogicDesign_SpaceUnit_h

#include "PaperUnit.hpp"
#include "Parameters.hpp"

class SpaceUnit
{
public:
    int mTemperature;           //空间的温度
    int mHeatCapacity;          //空间的比热容

    SpaceUnit();
        
    ~SpaceUnit();
};


#endif
