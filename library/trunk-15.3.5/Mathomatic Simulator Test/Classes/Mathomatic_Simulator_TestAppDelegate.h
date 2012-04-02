//
//  Mathomatic_Simulator_TestAppDelegate.h
//  Mathomatic Simulator Test
//
//  Created by Ben Gotow on 11/19/08.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//

#import <UIKit/UIKit.h>

@class Mathomatic_Simulator_TestViewController;

@interface Mathomatic_Simulator_TestAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    Mathomatic_Simulator_TestViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet Mathomatic_Simulator_TestViewController *viewController;

@end

