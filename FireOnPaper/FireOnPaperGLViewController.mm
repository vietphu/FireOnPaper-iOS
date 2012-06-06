//
//  FireOnPaperGLViewController.m
//  FireOnPaper
//
//  Created by Wang Zhongyu on 12-6-6.
//  Copyright (c) 2012年 Peking University. All rights reserved.
//

#import "FireOnPaperGLViewController.h"

@interface FireOnPaperGLViewController ()

@end

@implementation FireOnPaperGLViewController
@synthesize paperImage = _paperImage;

- (id)initWithNibName:(NSString *)nibNameOrNil
			   bundle:(NSBundle *)nibBundleOrNil
		  paperToFire:(UIImage *)paperToFire
{
	self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
		self.paperImage = paperToFire;
    }
    return self;
}

- (void)loadView
{
	self.view = [[[FireOnPaperGLView alloc] initWithFrame:[[UIScreen mainScreen] bounds] paperToFire:self.paperImage] autorelease];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view.
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
}

- (void)didReceiveMemoryWarning
{
	[self.view release];
}

- (void) viewWillDisappear:(BOOL)animated
{
	[self.view release];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

@end
