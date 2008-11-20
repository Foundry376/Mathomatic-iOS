//
//  Operation.h
//  mathomatic
//
//  Created by Ben Gotow on 10/25/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@class MathomaticViewController;

@interface Operation : NSObject {

    MathomaticViewController    * delegate;
    NSString                    * entry;
}

@property (nonatomic, retain) UIViewController * delegate;
@property (nonatomic, retain) NSString * entry;

- (id)initWithDelegate: (MathomaticViewController*)d andEntry:(NSString*)e;
- (void)dealloc;
- (void)run;

@end
