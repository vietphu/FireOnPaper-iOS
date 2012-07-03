//
//  FireOnPaperViewController.h
//  FireOnPaper
//
//  Created by Wang Zhongyu on 12-6-5.
//  Copyright (c) 2012年 Peking University. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <MediaPlayer/MediaPlayer.h>
#import <AVFoundation/AVFoundation.h>

@class FireOnPaperGLView;

@interface FireOnPaperViewController : UIViewController
<UIImagePickerControllerDelegate, UINavigationControllerDelegate> {
    AVAudioRecorder *recorder;
}

@property (retain, nonatomic) IBOutlet UIImageView *imageView;
@property (retain, nonatomic) IBOutlet UIButton *cameraButton;
@property (retain, nonatomic) IBOutlet UIButton *photoButton;
@property (retain, nonatomic) IBOutlet UIButton *resetButton;
@property (retain, nonatomic) IBOutlet UIButton *backButton;
@property (retain, nonatomic) IBOutlet UIButton *homeButton;
@property (retain, nonatomic) IBOutlet UIButton *menuButton;
@property (retain, nonatomic) IBOutlet UIButton *fireButton;
@property (retain, nonatomic) IBOutlet UIButton *defenceButton;
@property (retain, nonatomic) IBOutlet UIButton *show3dButton;

@property (strong, nonatomic) MPMoviePlayerController *moviePlayerController;
@property (strong, nonatomic) UIImage *image;
@property (strong, nonatomic) NSURL *movieURL;
@property (copy, nonatomic) NSString *lastChosenMediaType;
@property (assign, nonatomic) CGRect imageFrame;
@property (assign, nonatomic) BOOL imagePickerUsed;

@property (retain, nonatomic) FireOnPaperGLView *GLView;

- (IBAction)fireButtonSelected:(UIButton *)sender;
- (IBAction)cameraButtonSelected:(id)sender;
- (IBAction)photoButtonSelected:(id)sender;
- (IBAction)resetButtonSelected:(id)sender;
- (IBAction)backButtonSelected:(id)sender;
- (IBAction)homeButtonSelected:(id)sender;
- (IBAction)menuButtonSelected:(id)sender;

@end
