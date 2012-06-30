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
			// TODO bug to fix
            [recorder prepareToRecord];
            recorder.meteringEnabled = YES;
            [recorder record];
        }
		
		[self.view addGestureRecognizer:[[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(handlePinchGesture:)]];
		
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
	if (![UIImagePickerController isSourceTypeAvailable:
          UIImagePickerControllerSourceTypeCamera]) {
		cameraButton.enabled = NO;
		cameraButton.hidden = YES;
	}
    imageFrame = imageView.frame;
	self.imagePickerUsed = NO;
	self.backButton.hidden = YES;
	self.homeButton.hidden = YES;
	self.menuButton.hidden = YES;
}

- (void)viewDidUnload
{
	[self setResetButton:nil];
	[self setBackButton:nil];
	[self setHomeButton:nil];
	[self setMenuButton:nil];
	[self setFireButton:nil];
	[self setDefenceButton:nil];
	[super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
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
	BOOL isDefence = [Mode isEqualToString:[NSString stringWithFormat:@"Defence"]];
	[UIView beginAnimations:@"View Flip" context:nil];
	[UIView setAnimationDuration:1.25];
	[UIView setAnimationCurve:UIViewAnimationCurveEaseInOut];
	
	if (self.GLView.superview == nil) {
		CGSize itemSize = CGSizeMake(512, 512);
		UIGraphicsBeginImageContext(itemSize);
		CGRect imageRect = CGRectMake(0, 0, itemSize.width, itemSize.height);
		[imageView.image drawInRect:imageRect];
		imageView.image = UIGraphicsGetImageFromCurrentImageContext();
		UIGraphicsEndImageContext();
		if (self.GLView == nil) {
			self.GLView = [[FireOnPaperGLView alloc] initWithFrame:[[UIScreen mainScreen] bounds] paperToFire:imageView.image withRecorder:recorder FireOrDefence:isDefence];
		} else {
            [self.GLView reInitWithPaperToFire:imageView.image FireOrDefence:isDefence];
        }
		[self.view insertSubview:self.GLView atIndex:6];
	}
	
	[UIView setAnimationTransition: UIViewAnimationTransitionCurlUp
						   forView:self.view cache:NO];
	[UIView commitAnimations];
}

- (IBAction)fireButtonSelected:(UIButton *)sender
{
	[self SetupFire:sender.titleLabel.text];
}

- (IBAction)cameraButtonSelected:(id)sender
{
	[self getMediaFromSource:UIImagePickerControllerSourceTypeCamera];
}

- (IBAction)photoButtonSelected:(id)sender
{
	[self getMediaFromSource:UIImagePickerControllerSourceTypePhotoLibrary];
}

- (IBAction)backButtonSelected:(id)sender
{
	if (self.GLView.superview != nil) {
		[UIView beginAnimations:@"View Flip" context:nil];
		[UIView setAnimationDuration:1.25];
		[UIView setAnimationCurve:UIViewAnimationCurveEaseInOut];
		
		[self.GLView removeFromSuperview];
		
		[self.GLView stopRendering];
		
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
	imageView.image = [UIImage imageNamed:[NSString stringWithFormat:@"texture0.png"]];
}

#pragma mark  -
#pragma mark UIImagePickerController delegate methods
- (void)imagePickerController:(UIImagePickerController *)picker 
didFinishPickingMediaWithInfo:(NSDictionary *)info {
    self.lastChosenMediaType = [info objectForKey:UIImagePickerControllerMediaType];
    if ([lastChosenMediaType isEqual:(NSString *)kUTTypeImage]) {
        UIImage *resizedImage = [[self class] resizedImageWithInfo:info withWidthRatio:2.0 withHeightRatio:3.0];
        UIImage *shrunkenImage = shrinkImage(resizedImage, imageFrame.size);
        self.image = shrunkenImage;
    } else if ([lastChosenMediaType isEqual:(NSString *)kUTTypeMovie]) {
        self.movieURL = [info objectForKey:UIImagePickerControllerMediaURL];
    }
	self.imagePickerUsed = YES;
    [picker dismissModalViewControllerAnimated:YES];
}

- (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker {
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
	
	CGRect rect;
	if (width < height) {
		rect.size.height = CGImageGetHeight([resultImage CGImage]);
		rect.size.width = rect.size.height * width / height;
		rect.origin.x = rect.size.height * (1 - width/height) / 2.0;
		rect.origin.y = 0;
	}
	
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

- (void)handlePinchGesture:(UIPinchGestureRecognizer *)gesture
{
    if ((gesture.state == UIGestureRecognizerStateChanged) || (gesture.state == UIGestureRecognizerStateEnded)) {
		if (gesture.scale < 1 && gesture.numberOfTouches >= 2) [self backButtonSelected:nil];
	}
}

@end
