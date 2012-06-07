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
@property (retain, nonatomic) IBOutlet UIButton *resetButton;
@property (retain, nonatomic) IBOutlet UILabel *resetLabel;
@property (retain, nonatomic) IBOutlet UIButton *backButton;
@property (retain, nonatomic) IBOutlet UIButton *homeButton;
@property (retain, nonatomic) IBOutlet UIButton *menuButton;
@property (retain, nonatomic) IBOutlet UIButton *fireButton;

@property (strong, nonatomic) MPMoviePlayerController *moviePlayerController;
@property (strong, nonatomic) UIImage *image;
@property (strong, nonatomic) NSURL *movieURL;
@property (copy, nonatomic) NSString *lastChosenMediaType;
@property (assign, nonatomic) CGRect imageFrame;

@property (retain, nonatomic) FireOnPaperGLView *GLView;

- (IBAction)fireButtonSelected:(id)sender;
- (IBAction)cameraButtonSelected:(id)sender;
- (IBAction)photoButtonSelected:(id)sender;
- (IBAction)resetButtonSelected:(id)sender;
- (IBAction)backButtonSelected:(id)sender;
- (IBAction)homeButtonSelected:(id)sender;
- (IBAction)menuButtonSelected:(id)sender;

@end
