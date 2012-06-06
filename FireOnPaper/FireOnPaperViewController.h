//
//  FireOnPaperViewController.h
//  FireOnPaper
//
//  Created by Wang Zhongyu on 12-6-5.
//  Copyright (c) 2012年 Peking University. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <MediaPlayer/MediaPlayer.h>

@class FireOnPaperGLViewController;

@interface FireOnPaperViewController : UIViewController
	<UIImagePickerControllerDelegate, UINavigationControllerDelegate>

@property (strong, nonatomic) IBOutlet UIImageView *imageView;
@property (strong, nonatomic) IBOutlet UIBarButtonItem *cameraButton;
@property (strong, nonatomic) MPMoviePlayerController *moviePlayerController;
@property (strong, nonatomic) UIImage *image;
@property (strong, nonatomic) NSURL *movieURL;
@property (copy, nonatomic) NSString *lastChosenMediaType;
@property (assign, nonatomic) CGRect imageFrame;

@property (strong, nonatomic) FireOnPaperGLViewController *glViewController;
@property (retain, nonatomic) IBOutlet UILabel *crop_org_x;
@property (retain, nonatomic) IBOutlet UILabel *crop_org_y;
@property (retain, nonatomic) IBOutlet UILabel *crop_size_w;
@property (retain, nonatomic) IBOutlet UILabel *crop_size_h;

- (IBAction)paperButtonSelected:(id)sender;
- (IBAction)cameraButtonSelected:(id)sender;
- (IBAction)photoButtonSelected:(id)sender;
- (IBAction)stopButtonSelected:(id)sender;

@end
