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
+ (UIImage*)editedImageFromMediaWithInfo:(NSDictionary*)info;

@end

@implementation FireOnPaperViewController
@synthesize GLView;
@synthesize imageView;
@synthesize cameraButton;
@synthesize cameraLabel;
@synthesize photoButton;
@synthesize photoLabel;
@synthesize backButton;
@synthesize moviePlayerController;
@synthesize image;
@synthesize movieURL;
@synthesize lastChosenMediaType;
@synthesize imageFrame;
@synthesize defaultLabel;
@synthesize otherLabel;

#pragma mark  -
#pragma mark Override Methods
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
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
		cameraLabel.hidden = YES;
	}
	backButton.hidden = YES;
    imageFrame = imageView.frame;
}

- (void)viewDidUnload
{
	[super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
	[self setImageView:nil];
	[self setCameraButton:nil];
	[self setCameraLabel:nil];
	[self setPhotoButton:nil];
	[self setBackButton:nil];
	[self setDefaultLabel:nil];
	[self setPhotoLabel:nil];
	[self setOtherLabel:nil];
	[self setGLView:nil];
}

- (void)dealloc {
	[imageView release];
	[cameraButton release];
	[cameraLabel release];
	[photoButton release];
	[backButton release];
	[defaultLabel release];
	[photoLabel release];
	[otherLabel release];
	
	[moviePlayerController release];
	[image release];
	[movieURL release];
	[lastChosenMediaType release];
	
	[GLView release];
	
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
- (IBAction)paperButtonSelected:(id)sender
{
	[UIView beginAnimations:@"View Flip" context:nil];
	[UIView setAnimationDuration:1.25];
	[UIView setAnimationCurve:UIViewAnimationCurveEaseInOut];
	
	if (self.GLView.superview == nil) {
		if (self.GLView == nil) {
			CGSize itemSize = CGSizeMake(512, 512);
			UIGraphicsBeginImageContext(itemSize);
			CGRect imageRect = CGRectMake(0, 0, itemSize.width, itemSize.height);
			[imageView.image drawInRect:imageRect];
			imageView.image = UIGraphicsGetImageFromCurrentImageContext();
			UIGraphicsEndImageContext();
			self.GLView = [[FireOnPaperGLView alloc] initWithFrame:[[UIScreen mainScreen] bounds] paperToFire:imageView.image];
		}
		[self.view insertSubview:self.GLView atIndex:8];
		[self.GLView release];
	}
	
	[UIView setAnimationTransition: UIViewAnimationTransitionCurlUp
						   forView:self.view cache:NO];
	[UIView commitAnimations];
	
	backButton.hidden = NO;
}

- (IBAction)cameraButtonSelected:(id)sender
{
	[self getMediaFromSource:UIImagePickerControllerSourceTypeCamera];
}

- (IBAction)photoButtonSelected:(id)sender
{
	[self getMediaFromSource:UIImagePickerControllerSourceTypePhotoLibrary];
}

- (IBAction)stopButtonSelected:(id)sender
{
	[UIView beginAnimations:@"View Flip" context:nil];
	[UIView setAnimationDuration:1.25];
	[UIView setAnimationCurve:UIViewAnimationCurveEaseInOut];
	
	if (self.GLView.superview != nil) {
		[self.GLView removeFromSuperview];
		
		[self.GLView release];
		self.GLView = nil;
	}
	
	[UIView setAnimationTransition: UIViewAnimationTransitionCurlDown
						   forView:self.view cache:YES];
	[UIView commitAnimations];
	
	backButton.hidden = YES;
}

#pragma mark  -
#pragma mark UIImagePickerController delegate methods
- (void)imagePickerController:(UIImagePickerController *)picker 
didFinishPickingMediaWithInfo:(NSDictionary *)info {
    self.lastChosenMediaType = [info objectForKey:UIImagePickerControllerMediaType];
    if ([lastChosenMediaType isEqual:(NSString *)kUTTypeImage]) {
        //UIImage *chosenUneditedImage = [info objectForKey:UIImagePickerControllerOriginalImage];
		//UIImage *chosenImage = [info objectForKey:UIImagePickerControllerEditedImage];
        UIImage *resizedImage = [[self class] resizedImageWithInfo:info withWidthRatio:2.0 withHeightRatio:3.0];
        UIImage *shrunkenImage = shrinkImage(resizedImage, imageFrame.size);
        self.image = shrunkenImage;
    } else if ([lastChosenMediaType isEqual:(NSString *)kUTTypeMovie]) {
        self.movieURL = [info objectForKey:UIImagePickerControllerMediaURL];
    }
    [picker dismissModalViewControllerAnimated:YES];
}

- (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker {    
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

static inline double radians (double degrees) {return degrees * M_PI/180;}

+ (UIImage*)editedImageFromMediaWithInfo:(NSDictionary*)info{
    if(![info   objectForKey:UIImagePickerControllerCropRect])return nil;
    if(![info   objectForKey:UIImagePickerControllerOriginalImage])return nil;
    
    UIImage *originalImage=[info objectForKey:UIImagePickerControllerOriginalImage];
    CGRect rect=[[info objectForKey:UIImagePickerControllerCropRect] CGRectValue];
    
    CGImageRef imageRef = CGImageCreateWithImageInRect([originalImage CGImage], rect);
    
    CGBitmapInfo bitmapInfo = CGImageGetBitmapInfo(imageRef);
    CGColorSpaceRef colorSpaceInfo = CGImageGetColorSpace(imageRef);
    CGContextRef bitmap = CGBitmapContextCreate(NULL, rect.size.width, rect.size.height, CGImageGetBitsPerComponent(imageRef), CGImageGetBytesPerRow(imageRef), colorSpaceInfo, bitmapInfo);
    
    if (originalImage.imageOrientation == UIImageOrientationLeft) {
        CGContextRotateCTM (bitmap, radians(90));
        CGContextTranslateCTM (bitmap, 0, -rect.size.height);
        
    } else if (originalImage.imageOrientation == UIImageOrientationRight) {
        CGContextRotateCTM (bitmap, radians(-90));
        CGContextTranslateCTM (bitmap, -rect.size.width, 0);
        
    } else if (originalImage.imageOrientation == UIImageOrientationUp) {
        // NOTHING
    } else if (originalImage.imageOrientation == UIImageOrientationDown) {
        CGContextTranslateCTM (bitmap, rect.size.width, rect.size.height);
        CGContextRotateCTM (bitmap, radians(-180.));
    }
	
    CGContextDrawImage(bitmap, CGRectMake(0, 0, rect.size.width, rect.size.height), imageRef);
    CGImageRef ref = CGBitmapContextCreateImage(bitmap);
    
    UIImage *resultImage=[UIImage imageWithCGImage:ref];
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

@end
