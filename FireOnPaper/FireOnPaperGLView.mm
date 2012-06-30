//
//  FireOnPaperGLView.m
//  FireOnPaper
//
//  Created by Wang Zhongyu on 12-6-5.
//  Copyright (c) 2012年 Peking University. All rights reserved.
//

#import "FireOnPaperGLView.h"

@implementation FireOnPaperGLView

@synthesize motionManager;
@synthesize updateTimer;

bool boolMouseUp = false;
bool boolMouseMoving = false;
float mousePointX1 = 0;
float mousePointY1 = 0;
float mousePointX2 = 0;
float mousePointY2 = 0;

- (id)initWithFrame:(CGRect)frame
		paperToFire:(UIImage *)paperToFire
	   withRecorder:(AVAudioRecorder*)superRecorder
	  FireOrDefence:(BOOL)isDefence
{
    if (self = [super initWithFrame:frame]) {
        CAEAGLLayer* eaglLayer = (CAEAGLLayer*) super.layer;
        eaglLayer.opaque = YES;
        
        EAGLRenderingAPI api = kEAGLRenderingAPIOpenGLES2;
        m_context = [[EAGLContext alloc] initWithAPI:api];
        
        if (!m_context) {
            api = kEAGLRenderingAPIOpenGLES1;
            m_context = [[EAGLContext alloc] initWithAPI:api];
        }
        
        if (!m_context || ![EAGLContext setCurrentContext:m_context]) {
            [self release];
            return nil;
        }
		
        if (api == kEAGLRenderingAPIOpenGLES1) {
            NSLog(@"Using OpenGL ES 1.1");
			gl = new OpenGLES1::OpenGLES11Context();
        } else {
            NSLog(@"Using OpenGL ES 2.0");
			gl = new OpenGLES2::OpenGLES20Context();
        }
		
		m_width = CGRectGetWidth(frame);
		m_height = CGRectGetHeight(frame);
		
		[self reInitWithPaperToFire:paperToFire FireOrDefence:isDefence];
		
        //[self drawView: nil];
        m_timestamp = CACurrentMediaTime();
        
        CADisplayLink* displayLink;
        displayLink = [CADisplayLink displayLinkWithTarget:self
                                                  selector:@selector(drawView:)];
        [displayLink setFrameInterval:animationFrameInterval];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop]
                          forMode:NSDefaultRunLoopMode];
        
        [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
        
        [[NSNotificationCenter defaultCenter]
         addObserver:self
         selector:@selector(didRotate:)
         name:UIDeviceOrientationDidChangeNotification
         object:nil];
		
		self.motionManager = [[CMMotionManager alloc] init];
		
		if (motionManager.accelerometerAvailable) {
			motionManager.accelerometerUpdateInterval = 1.0 / 10.0;
			[motionManager startAccelerometerUpdates];
			self.updateTimer = [NSTimer scheduledTimerWithTimeInterval:1.0/10.0 target:self selector:@selector(updateDisplay) userInfo:nil repeats:YES];
		} else {
			NSLog(@"This device has no accelerometer.");
		}
        
        recorder = superRecorder;
		levelTimer = [NSTimer scheduledTimerWithTimeInterval: 0.04 target: self selector: @selector(levelTimerCallback:) userInfo: nil repeats: YES];
 
    }
    return self;
}

- (id)reInitWithPaperToFire:(UIImage *)paperToFire
			  FireOrDefence:(BOOL)isDefence
{
    if (m_engine != NULL)
    {
        delete m_engine;
        m_engine = NULL;
    }
	isDefenceMode = isDefence;
	
    // Generate and Bind Textures
	CGImageRef brushImage;
	CGContextRef brushContext;
	GLubyte	*brushData;
	size_t width, height;
	
	gl->glGenTextures(IMAGENUM, textureImageID);
	for (int i = 0; i<IMAGENUM; i++) {
		if (i == 0) brushImage = paperToFire.CGImage;
		else brushImage = [UIImage imageNamed:[NSString stringWithFormat:@"texture%d.png",i]].CGImage;
		
		width  = CGImageGetWidth(brushImage);
		height = CGImageGetHeight(brushImage);
		
		if (brushImage) {
			brushData = (GLubyte *) calloc(width * height * 4, sizeof(GLubyte));
			brushContext = CGBitmapContextCreate(brushData, width, height, 8, width * 4, CGImageGetColorSpace(brushImage), kCGImageAlphaPremultipliedLast);
			CGContextDrawImage(brushContext, CGRectMake(0.0, 0.0, (CGFloat)width, (CGFloat)height), brushImage);
			CGContextRelease(brushContext);
			
			gl->glBindTexture(GL_TEXTURE_2D, textureImageID[i]);
			
			gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			
			gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, brushData);
			free(brushData);
		}
		else
			NSLog(@"initWithFrame brushImage error!");
	}
	
    m_engine = new FireOnPaperEngine();
    m_engine->Initialize(m_width, m_height, gl, textureImageID, isDefenceMode);
    canBufferDestroyed = NO;
	
    return self;
}

- (id) stopRendering
{
    m_engine->SetStopRendering();
	gl->glDeleteTextures(IMAGENUM, textureImageID);
    return self;
}

+ (Class) layerClass
{
    return [CAEAGLLayer class];
}

- (void) drawView: (CADisplayLink *) displayLink
{
	[EAGLContext setCurrentContext:m_context];
	gl->glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
	
    if (displayLink != nil) {
        float elapseSeconds = displayLink.timestamp - m_timestamp;
        m_timestamp = displayLink.timestamp;
		NSLog(@"%lf FPS\n", 1.0 / elapseSeconds);
		m_engine->UpdateAnimation(elapseSeconds);
    }
	m_engine->Render();
	
	gl->glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffer);
    [m_context presentRenderbuffer:GL_RENDERBUFFER];
}

- (void) dealloc
{
	[self destroyFramebuffer];
	gl->glDeleteTextures(IMAGENUM, textureImageID);
	
    if ([EAGLContext currentContext] == m_context) {
        [EAGLContext setCurrentContext:nil];
    }
	[m_context release];
	
	delete m_engine;
	delete gl;

    [super dealloc];
}

- (void) didRotate:(NSNotification *)notification
{
    [self drawView: nil];
}

- (void)updateDisplay
{
	if (motionManager.accelerometerAvailable) {
        CMAccelerometerData *accelerometerData = motionManager.accelerometerData;
		m_engine->OnRotateWithAccelerometer(accelerometerData.acceleration.x, accelerometerData.acceleration.y, accelerometerData.acceleration.z);
        //NSLog(@"\n 时空转转转，我经历了悲喜:  x: %f y: %f z: %f \n",accelerometerData.acceleration.x, accelerometerData.acceleration.y, accelerometerData.acceleration.z);
	}
	[self drawView: nil];
}


- (BOOL)createFramebuffer
{
	// Generate IDs for a framebuffer object and a color renderbuffer
	gl->glGenFramebuffers(1, &m_framebuffer);
	gl->glGenRenderbuffers(1, &m_renderbuffer);
	
	gl->glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
	gl->glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffer);
	// This call associates the storage for the current render buffer with the EAGLDrawable (our CAEAGLLayer)
	// allowing us to draw into a buffer that will later be rendered to screen wherever the layer is (which corresponds with our view).
	[m_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(id<EAGLDrawable>)self.layer];
	gl->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_renderbuffer);
	
	gl->glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &m_width);
	gl->glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &m_height);
	
	gl->glGenRenderbuffers(1, &m_depthRenderbuffer);
	gl->glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
	gl->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, m_width, m_height);
	gl->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderbuffer);
	
	if(gl->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		NSLog(@"failed to make complete framebuffer object %x", gl->glCheckFramebufferStatus(GL_FRAMEBUFFER));
		return NO;
	}
	
	canBufferDestroyed = YES;
	
	return YES;
}

// Clean up any buffers we have allocated.
- (void)destroyFramebuffer
{
	if (canBufferDestroyed) {
		gl->glDeleteFramebuffers(1, &m_framebuffer);
		m_framebuffer = 0;
		gl->glDeleteRenderbuffers(1, &m_renderbuffer);
		m_renderbuffer = 0;
	
		if(m_depthRenderbuffer)
		{
			gl->glDeleteRenderbuffers(1, &m_depthRenderbuffer);
			m_depthRenderbuffer = 0;
		}
		canBufferDestroyed = NO;
	}
}

// If our view is resized, we'll be asked to layout subviews.
// This is the perfect opportunity to also update the framebuffer so that it is
// the same size as our display area.
-(void)layoutSubviews
{
	[EAGLContext setCurrentContext:m_context];
	[self destroyFramebuffer];
	[self createFramebuffer];
	[self performSelector:@selector(setupView)];
}

- (void)setupView
{
#define DEGREES_TO_RADIANS(__ANGLE) ((__ANGLE) / 180.0 * M_PI)
	const GLfloat zNear = 1.0, zFar = 100.0, fieldOfView = 80.0;
	GLfloat size= zNear * tanf(DEGREES_TO_RADIANS(fieldOfView) / 2.0);
	CGRect rect = self.bounds;
	
	gl->glViewport(0,0,m_width,m_height);
	gl->glMatrixMode(GL_PROJECTION);
	gl->glLoadIdentity();
	
	gl->glFrustumf(-size, size, -size/(rect.size.width/rect.size.height),size/(rect.size.width/rect.size.height), zNear,zFar);
	
	gl->glMatrixMode(GL_MODELVIEW);
	gl->glLoadIdentity();
}

//使用一个回调函数，来对声音做出反映
- (void)levelTimerCallback:(NSTimer *)timer {
	[recorder updateMeters];
    
	const double ALPHA = 0.10;
	double peakPowerForChannel = pow(10, (0.05 * [recorder peakPowerForChannel:0]));
	lowPassResults = ALPHA * peakPowerForChannel + (1.0 - ALPHA) * lowPassResults;	
	
	if (lowPassResults < 0.75)
    {
		
    }
    else
    {
        m_engine->OnDisturbWithMicrophone(false, true, lowPassResults * 1.0 / 0.75);
        //NSLog(@"\n 树欲静而风不止:    %f \n",lowPassResults);
    }
}

// Handles the start of a touch
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	//CGRect bounds = [self bounds];
    UITouch * touch = [[event touchesForView:self] anyObject];
	
	CGPoint location = [touch locationInView:self];
	
	mousePointX1 = location.x/((float)m_width);
	mousePointY1 = location.y/((float)m_height);
	
	if (isDefenceMode) m_engine->OnTouchChangePaperMaterial(mousePointX1, mousePointY1);
	else m_engine->OnTouchPaperToBurn(mousePointX1, mousePointY1);

	//NSLog(@"touches Begin at x = %f, y = %f", location.x, location.y);
	//NSLog(@"touches Begin at x = %f, y = %f", mousePointX1, mousePointY1);
}

// Handles the continuation of a touch.
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{  
	
	//CGRect bounds = [self bounds];
	UITouch * touch = [[event touchesForView:self] anyObject];
	CGPoint location = [touch locationInView:self];
	
	boolMouseMoving=true;
	
	mousePointX2 = location.x/((float)m_width);
	mousePointY2 = location.y/((float)m_height);
	
    if (isDefenceMode) m_engine->OnTouchChangePaperMaterial(mousePointX2, mousePointY2);
    else m_engine->OnTouchPaperToBurn(mousePointX2, mousePointY2);
    
	//NSLog(@"touches Move at x = %f, y = %f", location.x, location.y);
	//NSLog(@"touches Move at x = %f, y = %f", mousePointX2, mousePointY2);
}

// Handles the end of a touch event when the touch is a tap.
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	//处理单击事件
	//CGRect bounds = [self bounds];
    UITouch * touch = [[event touchesForView:self] anyObject];
	CGPoint location = [touch locationInView:self];
	
	boolMouseUp = true;
	mousePointX1 = mousePointX2 = location.x/((float)m_width);
	mousePointY1 = mousePointY2 = location.y/((float)m_height);
	boolMouseMoving = false;
	
	//NSLog(@"touches End at x = %f, y = %f", location.x, location.y);
	//NSLog(@"touches End at x = %f, y = %f", mousePointX1, mousePointY1);
}

// Handles the end of a touch event.
- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	// If appropriate, add code necessary to save the state of the application.
	// This application is not saving state.
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
}
*/

@end
