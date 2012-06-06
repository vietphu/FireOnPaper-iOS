//
//  FireOnPaperViewController.m
//  FireOnPaper
//
//  Created by Wang Zhongyu on 12-6-5.
//  Copyright (c) 2012年 Peking University. All rights reserved.
//

#import "FireOnPaperViewController.h"
#import "FireOnPaperGLViewController.h"
#import <MobileCoreServices/UTCoreTypes.h>

@interface FireOnPaperViewController ()

static UIImage *shrinkImage(UIImage *original, CGSize size);
- (void)getMediaFromSource:(UIImagePickerControllerSourceType)sourceType;
- (void)updateDisplay;
- (UIImage*)editedImageFromMediaWithInfo:(NSDictionary*)info;

@end

@implementation FireOnPaperViewController
@synthesize imageView;
@synthesize cameraButton;
@synthesize moviePlayerController;
@synthesize image;
@synthesize movieURL;
@synthesize lastChosenMediaType;
@synthesize imageFrame;
@synthesize glViewController;

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
    }
    imageFrame = imageView.frame;
	imageView.image = [UIImage imageNamed:@"texture0.png"];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
    self.imageView = nil;
    self.cameraButton = nil;
    self.moviePlayerController = nil;
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
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

- (IBAction)paperButtonSelected:(id)sender
{
	if (self.glViewController.view.superview == nil) {
		if (self.glViewController == nil) {
			CGSize itemSize = CGSizeMake(512, 512);
			UIGraphicsBeginImageContext(itemSize);
			CGRect imageRect = CGRectMake(0, 0, itemSize.width, itemSize.height);
			[imageView.image drawInRect:imageRect];
			imageView.image = UIGraphicsGetImageFromCurrentImageContext();
			UIGraphicsEndImageContext();
			self.glViewController = [[[FireOnPaperGLViewController alloc] initWithNibName:@"FireOnPaperGLViewController" bundle:nil paperToFire:imageView.image] autorelease];
		}
		[self.view insertSubview:self.glViewController.view atIndex:10000];
	}
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
	if (self.glViewController.view.superview != nil) {
		[self.glViewController.view removeFromSuperview];
		[self.glViewController release];
		self.glViewController = nil;
	}
}

#pragma mark UIImagePickerController delegate methods
- (void)imagePickerController:(UIImagePickerController *)picker 
didFinishPickingMediaWithInfo:(NSDictionary *)info {
    self.lastChosenMediaType = [info objectForKey:UIImagePickerControllerMediaType];
    if ([lastChosenMediaType isEqual:(NSString *)kUTTypeImage]) {
        //UIImage *chosenUneditedImage = [info objectForKey:UIImagePickerControllerOriginalImage];
		//UIImage *chosenImage = [info objectForKey:UIImagePickerControllerEditedImage];
        UIImage *resizedImage = [self editedImageFromMediaWithInfo:info];
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

static inline double radians (double degrees) {return degrees * M_PI/180;}

- (UIImage*)editedImageFromMediaWithInfo:(NSDictionary*)info{
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
    
	rect.size.height = CGImageGetHeight([resultImage CGImage]);
	rect.size.width = rect.size.height * 2.0 / 3.0;
	rect.origin.x = rect.size.height / 6.0;
	rect.origin.y = 0;
	
	imageRef = CGImageCreateWithImageInRect([resultImage CGImage], rect);
    
    bitmapInfo = CGImageGetBitmapInfo(imageRef);
    colorSpaceInfo = CGImageGetColorSpace(imageRef);
    bitmap = CGBitmapContextCreate(NULL, rect.size.width, rect.size.height, CGImageGetBitsPerComponent(imageRef), CGImageGetBytesPerRow(imageRef), colorSpaceInfo, bitmapInfo);
	CGContextDrawImage(bitmap, CGRectMake(0, 0, rect.size.width, rect.size.height), imageRef);
    ref = CGBitmapContextCreateImage(bitmap);
    resultImage=[UIImage imageWithCGImage:ref];
    CGImageRelease(imageRef);
    CGContextRelease(bitmap);
    CGImageRelease(ref);
	
    return resultImage;
}

#pragma mark  -
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

- (void)dealloc {
    [super dealloc];
}
@end
