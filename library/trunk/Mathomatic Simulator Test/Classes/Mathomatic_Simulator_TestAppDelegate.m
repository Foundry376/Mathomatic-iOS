//
//  Mathomatic_Simulator_TestAppDelegate.m
//  Mathomatic Simulator Test
//
//  Created by Ben Gotow on 11/19/08.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//

#import "Mathomatic_Simulator_TestAppDelegate.h"
#import "Mathomatic_Simulator_TestViewController.h"

@implementation Mathomatic_Simulator_TestAppDelegate

@synthesize window;
@synthesize viewController;


- (void)applicationDidFinishLaunching:(UIApplication *)application {    
    
    // Override point for customization after app launch    
    [window addSubview:viewController.view];
    [window makeKeyAndVisible];
}


- (void)dealloc {
    [viewController release];
    [window release];
    [super dealloc];
}


@end
