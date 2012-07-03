//
//  Parameters.h
//  LogicDesign
//
//  Created by Han John on 12-4-14.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef LogicDesign_Parameters_h
#define LogicDesign_Parameters_h

#define INVALID -948155

#define PAPER_LOGIC_WIDTH 200//100      // % 10 should be 0 纸片的逻辑宽度
#define PAPER_LOGIC_HEIGHT 300//150     // % 10 should be 0 纸片的逻辑高度

#define SPACE_LOGIC_WIDTH 120//((PAPER_LOGIC_WIDTH)/5*6) //空间的逻辑宽度
#define SPACE_LOGIC_HEIGHT 180//((PAPER_LOGIC_HEIGHT)/5*6) //空间的逻辑高度

#define PAPER_SPACE_ADJUST_WIDTH 10//((PAPER_LOGIC_WIDTH)/10) //纸片距空间边缘的逻辑宽度
#define PAPER_SPACE_ADJUST_HEIGHT 15//((PAPER_LOGIC_HEIGHT)/10) //纸片距空间边缘的逻辑高度

#define SCREEN_WIDTH 30.0f //屏幕的实际宽度尺寸
#define SCREEN_HEIGHT 45.0f //屏幕的实际高度尺寸

#define SCREEN_WIDTH_PER_UNIT ((SCREEN_WIDTH)/(float)(SPACE_LOGIC_WIDTH)) //每一个屏幕逻辑宽度对应的实际宽度
#define SCREEN_HEIGHT_PER_UNIT ((SCREEN_HEIGHT)/(float)(SPACE_LOGIC_HEIGHT)) //每一个屏幕逻辑高度对应的实际高度
#define SCREEN_X_PADDING ((SCREEN_WIDTH)/12.0f) //纸片在宽度方向的实际距边缘值
#define SCREEN_Y_PADDING ((SCREEN_HEIGHT)/12.0f) //纸片在高度方向的实际距边缘值

#define PAPER_WIDTH ((SCREEN_WIDTH)/12.0f*10.0f) //纸片实际宽度
#define PAPER_HEIGHT ((SCREEN_HEIGHT)/12.0f*10.0f) //纸片实际高度
#define PAPER_LEFT_TOP_X ((-SCREEN_WIDTH)/2 + (SCREEN_X_PADDING)) //纸片左上角的实际x坐标值
#define PAPER_LEFT_TOP_Y ((SCREEN_HEIGHT)/2 - (SCREEN_Y_PADDING)) //纸片右上角的实际y坐标值

#define TPRT_ROOM 300 //室温值
#define TPRT_NORMALPAPER_IGNITION 456 //纸片燃点温度值

#define HEAT_PER_PHLOGISTON 120.0//120 每一个燃素单元的消耗可以产生的热量值
#define HEAT_SPREADRATIO_SPACE_TO_SPACE 0.15//0.06 空间到空间的热传导指数
#define HEAT_SPREADRATIO_FIRE_TO_SPACE 30.0//25.0 火焰到空间的热传导指数
#define HEAT_SPREADRATIO_SPACE_TO_SPACE_DOWN 0.02 //0.02 空间到其下方空间的热传导系数 目前已废弃
#define HEAT_LOSSRATIO (0.000000000008)//(0.000000000008) 热辐射散失系数

//25 0.02 0.08 8.5e-11

#define HEAT_CAPACITY_SPACE 1 //空间比热容

#define TOUCH_SET_PHLOGISTON_DENSITY 1000 //触摸设置的燃素含量

#define NORMALPAPER_PHLOGISTON_DENSITY 6//16 初始每一个纸片单元的燃素含量
//40
#define NORMALPAPER_PHLOGISTON_CONSUMERATE 1 //燃素消耗速度
#define NORMALPAPER_MASS 1 //纸片质量

#endif
