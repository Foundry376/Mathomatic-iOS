//
//  Command.h
//  mathomatic
//
//  Created by Ben Gotow on 10/24/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "ExpressionScrollView.h"

@interface MathCommand : NSObject {
    NSMutableArray * input;
    NSMutableArray * output;
    BOOL showInput;
    
    NSMutableArray * outputEquations;
}

@property (assign) BOOL showInput;
@property (nonatomic, retain) NSMutableArray * input;
@property (nonatomic, retain) NSMutableArray * output;

- (void)addInput:(NSString *)item;
- (BOOL)run;
- (void)dealloc;

- (NSArray*)outputEquations;
- (BOOL)output:(NSString*)line contains:(NSString*)string;

- (int)attachedHeight;
- (void)attachToView: (UIView*)v;

@end
