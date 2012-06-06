//
//  FireOnPaperGLView.h
//  FireOnPaper
//
//  Created by Wang Zhongyu on 12-6-5.
//  Copyright (c) 2012年 Peking University. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <CoreMotion/CoreMotion.h>
#import <AVFoundation/AVFoundation.h>
#import <CoreAudio/CoreAudioTypes.h>
#import "mach/mach_time.h"
#include "OpenGLES11Context.hpp"
#include "OpenGLES20Context.hpp"
#include "FireOnPaperEngine.hpp"

using namespace OpenGLES;

#define IMAGENUM 3

@interface FireOnPaperGLView : UIView {
	OpenGLES::OpenGLESContext *gl;
	EAGLContext * m_context;
	GLint m_width;
	GLint m_height;
	
    GLuint m_framebuffer;
    GLuint m_renderbuffer;
	GLuint m_depthRenderbuffer;
	BOOL canBufferDestroyed;
	GLuint textureImageID[IMAGENUM];
	
	FireOnPaperEngine * m_engine;
	
	NSInteger animationFrameInterval;
    float m_timestamp;
    AVAudioRecorder *recorder;
	NSTimer *levelTimer;
	double lowPassResults;
}

@property (retain) CMMotionManager * motionManager;
@property (retain) NSTimer * updateTimer;

- (void) drawView: (CADisplayLink *) displayLink;
- (void) didRotate: (NSNotification *) notification;

- (void)levelTimerCallback:(NSTimer *)timer;

@end
