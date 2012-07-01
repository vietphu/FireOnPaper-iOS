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
	
	// gles-bc
	OpenGLES::OpenGLESContext *gl;
	// context for OpenGLES on iOS
	EAGLContext * m_context;
	
	// screen width and screen height
	GLint m_width;
	GLint m_height;
	
	// defence mode or fire mode
	BOOL isDefenceMode;
	
	// buffers
    GLuint m_framebuffer;
    GLuint m_renderbuffer;
	GLuint m_depthRenderbuffer;
	
	// boolean of buffer initialized or not
	BOOL canBufferDestroyed;
	// texture IDs
	GLuint textureImageID[IMAGENUM];
	
	// fire on paper engine
	FireOnPaperEngine * m_engine;
	
	// Hardware attributes for sensor
	NSInteger animationFrameInterval;
    float m_timestamp;
    AVAudioRecorder *recorder;
	NSTimer *levelTimer;
	double lowPassResults;
}

@property (retain) CMMotionManager * motionManager;
@property (retain) NSTimer * updateTimer;

// initialization
- (id) initWithFrame:(CGRect)frame
		 paperToFire:(UIImage *)paperToFire
		withRecorder:(AVAudioRecorder*)superRecorder
	   FireOrDefence:(BOOL)isDefence;

// reinitialization
- (id) reInitWithPaperToFire:(UIImage *)paperToFire
			   FireOrDefence:(BOOL)isDefence;
// stop rendering
- (id) stopRendering;

- (void) drawView: (CADisplayLink *) displayLink;
- (void) didRotate: (NSNotification *) notification;

- (void)levelTimerCallback:(NSTimer *)timer;

@end
