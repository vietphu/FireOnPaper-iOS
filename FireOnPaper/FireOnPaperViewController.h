//
//  FireOnPaperViewController.h
//  FireOnPaper
//
//  Created by Wang Zhongyu on 12-6-5.
//  Copyright (c) 2012年 Peking University. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <MediaPlayer/MediaPlayer.h>

@class FireOnPaperGLView;

@interface FireOnPaperViewController : UIViewController
	<UIImagePickerControllerDelegate, UINavigationControllerDelegate>

@property (retain, nonatomic) IBOutlet UIImageView *imageView;
@property (retain, nonatomic) IBOutlet UILabel *defaultLabel;
@property (retain, nonatomic) IBOutlet UILabel *otherLabel;
@property (retain, nonatomic) IBOutlet UIButton *cameraButton;
@property (retain, nonatomic) IBOutlet UILabel *cameraLabel;
@property (retain, nonatomic) IBOutlet UIButton *photoButton;
@property (retain, nonatomic) IBOutlet UILabel *photoLabel;
@property (retain, nonatomic) IBOutlet UIButton *backButton;

@property (strong, nonatomic) MPMoviePlayerController *moviePlayerController;
@property (strong, nonatomic) UIImage *image;
@property (strong, nonatomic) NSURL *movieURL;
@property (copy, nonatomic) NSString *lastChosenMediaType;
@property (assign, nonatomic) CGRect imageFrame;

@property (retain, nonatomic) FireOnPaperGLView *GLView;

- (IBAction)paperButtonSelected:(id)sender;
- (IBAction)cameraButtonSelected:(id)sender;
- (IBAction)photoButtonSelected:(id)sender;
- (IBAction)stopButtonSelected:(id)sender;

@end
