//
//  FireOnPaperAppDelegate.h
//  FireOnPaper
//
//  Created by Wang Zhongyu on 12-6-5.
//  Copyright (c) 2012年 Peking University. All rights reserved.
//

#import <UIKit/UIKit.h>

@class FireOnPaperViewController;
@class FireOnPaperGLViewController;

@interface FireOnPaperAppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;
@property (strong, nonatomic) FireOnPaperViewController *viewController;
@property (strong, nonatomic) FireOnPaperGLViewController * glViewController;

@end
