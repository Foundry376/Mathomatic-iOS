//
//  mathomaticAppDelegate.h
//  mathomatic
//
//  Created by Ben Gotow on 10/23/08.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//

#import <UIKit/UIKit.h>

@class MathomaticViewController;

@interface MathomaticAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    MathomaticViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet MathomaticViewController *viewController;

@end

