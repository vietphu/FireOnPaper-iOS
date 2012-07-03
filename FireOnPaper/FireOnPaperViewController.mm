//
//  FireOnPaperViewController.m
//  FireOnPaper
//
//  Created by Wang Zhongyu on 12-6-5.
//  Copyright (c) 2012年 Peking University. All rights reserved.
//

#import "FireOnPaperViewController.h"
#import "FireOnPaperGLView.h"
#import <MobileCoreServices/UTCoreTypes.h>

@interface FireOnPaperViewController ()

static UIImage *shrinkImage(UIImage *original, CGSize size);
- (void)getMediaFromSource:(UIImagePickerControllerSourceType)sourceType;
- (void)updateDisplay;
+ (UIImage*)resizedImageWithInfo:(NSDictionary *)info
				  withWidthRatio:(GLfloat)width
				 withHeightRatio:(GLfloat)height;

@end

@implementation FireOnPaperViewController
@synthesize imagePickerUsed;
@synthesize GLView;
@synthesize imageView;
@synthesize cameraButton;
@synthesize photoButton;
@synthesize resetButton;
@synthesize backButton;
@synthesize homeButton;
@synthesize menuButton;
@synthesize fireButton;
@synthesize defenceButton;
@synthesize show3dButton;
@synthesize moviePlayerController;
@synthesize image;
@synthesize movieURL;
@synthesize lastChosenMediaType;
@synthesize imageFrame;

#pragma mark  -
#pragma mark Override Methods
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
		
		//LC Sound
        NSURL *url = [NSURL fileURLWithPath:@"/dev/null"];
        NSDictionary *settings = [NSDictionary dictionaryWithObjectsAndKeys:
                                  [NSNumber numberWithFloat: 44100.0],                 AVSampleRateKey,
                                  [NSNumber numberWithInt: kAudioFormatAppleLossless], AVFormatIDKey,
                                  [NSNumber numberWithInt: 1],                         AVNumberOfChannelsKey,
                                  [NSNumber numberWithInt: AVAudioQualityMax],         AVEncoderAudioQualityKey,
                                  nil];
        NSError *error;
		recorder = [[AVAudioRecorder alloc] initWithURL:url settings:settings error:&error];
        if (recorder) {
            [recorder prepareToRecord];
            recorder.meteringEnabled = YES;
            [recorder record];
        }
		
		// add gesture recognizer with function of handlePinchGesture
		[self.view addGestureRecognizer:[[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(handlePinchGesture:)]];
		
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
	// decide if display camera button depending on device hardware
	if (![UIImagePickerController isSourceTypeAvailable:
          UIImagePickerControllerSourceTypeCamera]) {
		cameraButton.enabled = NO;
		cameraButton.hidden = YES;
	}
	// initialize the views
    imageFrame = imageView.frame;
	self.imagePickerUsed = NO;
	self.backButton.hidden = YES;
	self.homeButton.hidden = YES;
	self.menuButton.hidden = YES;
}

- (void)viewDidUnload
{
	[super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
	[self setResetButton:nil];
	[self setBackButton:nil];
	[self setHomeButton:nil];
	[self setMenuButton:nil];
	[self setFireButton:nil];
	[self setDefenceButton:nil];
    [self setShow3dButton:nil];
	[self setImageView:nil];
	[self setCameraButton:nil];
	[self setPhotoButton:nil];
	[self setGLView:nil];
}

- (void)dealloc {
	[imageView release];
	[cameraButton release];
	[photoButton release];

	[moviePlayerController release];
	[image release];
	[movieURL release];
	[lastChosenMediaType release];
	
	[GLView release];
	
	[resetButton release];
	[backButton release];
	[homeButton release];
	[menuButton release];
	[fireButton release];
	[defenceButton release];
    [show3dButton release];
    [super dealloc];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    [self updateDisplay];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

#pragma mark  -
#pragma mark Action for Buttons

- (void)SetupFire:(NSString *)Mode
{
	// start animation
	[UIView beginAnimations:@"View Flip" context:nil];
	[UIView setAnimationDuration:1.25];
	[UIView setAnimationCurve:UIViewAnimationCurveEaseInOut];
	
	if (self.GLView.superview == nil) {
		// get image to fire
		CGSize itemSize = CGSizeMake(512, 512);
		UIGraphicsBeginImageContext(itemSize);
		CGRect imageRect = CGRectMake(0, 0, itemSize.width, itemSize.height);
		[imageView.image drawInRect:imageRect];
		imageView.image = UIGraphicsGetImageFromCurrentImageContext();
		UIGraphicsEndImageContext();
		if (self.GLView == nil) { // alloc and initialize the fire on paper view
			self.GLView = [[FireOnPaperGLView alloc] initWithFrame:[[UIScreen mainScreen] bounds] paperToFire:imageView.image withRecorder:recorder withPlayMode:Mode];
		} else { // reinitialize the fire on paper view
            [self.GLView reInitWithPaperToFire:imageView.image withPlayMode:Mode];
        }
		// display the fire on paper view
		[self.view insertSubview:self.GLView atIndex:100];
	}
	
	// end animation
	[UIView setAnimationTransition: UIViewAnimationTransitionCurlUp
						   forView:self.view cache:NO];
	[UIView commitAnimations];
}

- (IBAction)fireButtonSelected:(UIButton *)sender
{
	// fire mode
	[self SetupFire:sender.titleLabel.text];
}

- (IBAction)cameraButtonSelected:(id)sender
{
	// use camera to get photo
	[self getMediaFromSource:UIImagePickerControllerSourceTypeCamera];
}

- (IBAction)photoButtonSelected:(id)sender
{
	// use photo library to get photo
	[self getMediaFromSource:UIImagePickerControllerSourceTypePhotoLibrary];
}

- (IBAction)backButtonSelected:(id)sender
{
	if (self.GLView.superview != nil) {
		// start animation
		[UIView beginAnimations:@"View Flip" context:nil];
		[UIView setAnimationDuration:1.25];
		[UIView setAnimationCurve:UIViewAnimationCurveEaseInOut];
		
		// remove the fire on paper view
		[self.GLView removeFromSuperview];
		
		// stop rendering the fire on paper view
		[self.GLView stopRendering];
		
		// end animation
		[UIView setAnimationTransition: UIViewAnimationTransitionCurlDown
							   forView:self.view cache:YES];
		[UIView commitAnimations];
	}
}

- (IBAction)homeButtonSelected:(id)sender {
	[self backButtonSelected:sender];
}

- (IBAction)menuButtonSelected:(id)sender {
	[self getMediaFromSource:UIImagePickerControllerSourceTypePhotoLibrary];
}

- (IBAction)resetButtonSelected:(id)sender {
	// reset the default image to fire
	imageView.image = [UIImage imageNamed:[NSString stringWithFormat:@"texture0.png"]];
}

#pragma mark  -
#pragma mark UIImagePickerController delegate methods
- (void)imagePickerController:(UIImagePickerController *)picker 
didFinishPickingMediaWithInfo:(NSDictionary *)info {
	// get last chosen photo or video
    self.lastChosenMediaType = [info objectForKey:UIImagePickerControllerMediaType];
    if ([lastChosenMediaType isEqual:(NSString *)kUTTypeImage]) {
		// resize the chose photo with size of 2:3
        UIImage *resizedImage = [[self class] resizedImageWithInfo:info withWidthRatio:2.0 withHeightRatio:3.0];
        UIImage *shrunkenImage = shrinkImage(resizedImage, imageFrame.size);
        self.image = shrunkenImage;
    } else if ([lastChosenMediaType isEqual:(NSString *)kUTTypeMovie]) {
        self.movieURL = [info objectForKey:UIImagePickerControllerMediaURL];
    }
	// end picker
	self.imagePickerUsed = YES;
    [picker dismissModalViewControllerAnimated:YES];
}

- (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker {
	// end picker
	self.imagePickerUsed = NO;
    [picker dismissModalViewControllerAnimated:YES];
}

#pragma mark  -
#pragma mark Edit image's size
+ (UIImage*)resizedImageWithInfo:(NSDictionary *)info
				  withWidthRatio:(GLfloat)width
				 withHeightRatio:(GLfloat)height
{
	UIImage * resultImage = [info objectForKey:UIImagePickerControllerEditedImage];
	
	// resize the rectangle of the photo
	CGRect rect;
	if (width < height) {
		rect.size.height = CGImageGetHeight([resultImage CGImage]);
		rect.size.width = rect.size.height * width / height;
		rect.origin.x = rect.size.height * (1 - width/height) / 2.0;
		rect.origin.y = 0;
	}
	
	// resize the photo with the rectangle above
	CGImageRef imageRef = CGImageCreateWithImageInRect([resultImage CGImage], rect);
    
    CGBitmapInfo bitmapInfo = CGImageGetBitmapInfo(imageRef);
    CGColorSpaceRef colorSpaceInfo = CGImageGetColorSpace(imageRef);
    CGContextRef bitmap = CGBitmapContextCreate(NULL, rect.size.width, rect.size.height, CGImageGetBitsPerComponent(imageRef), CGImageGetBytesPerRow(imageRef), colorSpaceInfo, bitmapInfo);
	CGContextDrawImage(bitmap, CGRectMake(0, 0, rect.size.width, rect.size.height), imageRef);
    CGImageRef ref = CGBitmapContextCreateImage(bitmap);
    resultImage=[UIImage imageWithCGImage:ref];
    CGImageRelease(imageRef);
    CGContextRelease(bitmap);
    CGImageRelease(ref);
	
	return resultImage;
}

#pragma mark  -
#pragma mark Setup UIImagePickerController
static UIImage *shrinkImage(UIImage *original, CGSize size) {
	
	// shrink image from orignial to new with size of "size"
    CGFloat scale = [UIScreen mainScreen].scale;
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    
    CGContextRef context = CGBitmapContextCreate(NULL, size.width * scale,
                                                 size.height * scale, 8, 0, colorSpace, kCGImageAlphaPremultipliedFirst);
    CGContextDrawImage(context,
                       CGRectMake(0, 0, size.width * scale, size.height * scale),
                       original.CGImage);
    CGImageRef shrunken = CGBitmapContextCreateImage(context);
    UIImage *final = [UIImage imageWithCGImage:shrunken];
    
    CGContextRelease(context);
    CGImageRelease(shrunken);	
    
    return final;
}

- (void)updateDisplay {
	// update the image view displaying image to fire
	if (!self.imagePickerUsed) return ;
	self.imagePickerUsed = NO;
    if ([lastChosenMediaType isEqual:(NSString *)kUTTypeImage]) {
        imageView.image = image;
        imageView.hidden = NO;
        moviePlayerController.view.hidden = YES;
    } else if ([lastChosenMediaType isEqual:(NSString *)kUTTypeMovie]) {
        [self.moviePlayerController.view removeFromSuperview];
        self.moviePlayerController = [[MPMoviePlayerController alloc]
                                      initWithContentURL:movieURL];
        moviePlayerController.view.frame = imageFrame;
        moviePlayerController.view.clipsToBounds = YES;
        [self.view addSubview:moviePlayerController.view];
        imageView.hidden = YES;
    }
}

- (void)getMediaFromSource:(UIImagePickerControllerSourceType)sourceType {
	// get Media from camera or photo library
    NSArray *mediaTypes = [UIImagePickerController
                           availableMediaTypesForSourceType:sourceType];
    if ([UIImagePickerController isSourceTypeAvailable:
         sourceType] && [mediaTypes count] > 0) {
        NSArray *mediaTypes = [UIImagePickerController
                               availableMediaTypesForSourceType:sourceType];
        UIImagePickerController *picker =
        [[UIImagePickerController alloc] init];
        picker.mediaTypes = mediaTypes;
        picker.delegate = self;
        picker.allowsEditing = YES;
        picker.sourceType = sourceType;
        [self presentModalViewController:picker animated:YES];
    } else {
        UIAlertView *alert = [[UIAlertView alloc] 
                              initWithTitle:@"Error accessing media" 
                              message:@"Device doesn’t support that media source." 
                              delegate:nil 
                              cancelButtonTitle:@"Drat!" 
                              otherButtonTitles:nil];
        [alert show];
    }
}

// Pinch Gesture ==> back
- (void)handlePinchGesture:(UIPinchGestureRecognizer *)gesture
{
    if ((gesture.state == UIGestureRecognizerStateChanged) || (gesture.state == UIGestureRecognizerStateEnded)) {
		if (gesture.scale < 1 && gesture.numberOfTouches >= 2) [self backButtonSelected:nil];
	}
}

@end
