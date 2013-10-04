//
//  mathomaticAppDelegate.m
//  mathomatic
//
//  Created by Ben Gotow on 10/23/08.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//

#import "MathomaticAppDelegate.h"
#import "MathomaticViewController.h"
#import "Mathomatic.h"

@implementation MathomaticAppDelegate

@synthesize window;
@synthesize viewController;


- (void)applicationDidFinishLaunching:(UIApplication *)application
{
    self.viewController = [[MathomaticViewController alloc] init];

    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    [window setRootViewController: self.viewController];
    [window makeKeyAndVisible];
    
    // call matho_init and make sure we're ready to start. This also clears any old steps and output,
    // so re-performing doesn't have any averse affects.
    matho_init();
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    [viewController save];
}

- (void)dealloc
{
    [viewController release];
    [window release];
    [super dealloc];
}


@end
